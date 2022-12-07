#include "game.h"
#include "utils.h"

BOOL namedpipes_sync(Game* game);
DWORD WINAPI clientMsg(LPVOID param);
DWORD WINAPI client(LPVOID param);

BOOL init_nem_and_sync(Game* game) {
	game->map_file = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMem), SHM);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		Log((TCHAR*)_T("Já exite um servidor iniciado"));
		return FALSE;
	}

	game->shared_mem = (SharedMem*)MapViewOfFile(game->map_file, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(SharedMem));
	game->shared_mem->rP = 0;
	game->shared_mem->wP = 0;

	game->mutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	if (game->mutex == NULL) {
		Log((TCHAR*)_T("Não foi possivel inicar o mutex"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		return FALSE;
	}

	game->event = CreateEvent(NULL, FALSE, FALSE, EVENT_NAME);
	if (game->event == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o evento"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		return FALSE;
	}

	game->sem_read = CreateSemaphore(NULL, 0, BUFFERSIZE, SEMAPHORE_READ_NAME);

	if (game->sem_read == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o semáforo para leitura"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		return FALSE;
	}

	game->sem_write = CreateSemaphore(NULL, BUFFERSIZE, BUFFERSIZE, SEMAPHORE_WRITE_NAME);

	if (game->sem_write == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o semáforo para Escrita"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		CloseHandle(game->sem_read);
		return FALSE;
	}

	game->shut_down_event = CreateEvent(NULL, TRUE, FALSE, SHUT_DOWN);
	if (game->shut_down_event == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o evento"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		CloseHandle(game->sem_write);
		CloseHandle(game->sem_read);
		return FALSE;
	}

	Log((TCHAR*)_T("Handles criados"));
	return namedpipes_sync(game);;
}

BOOL namedpipes_sync(Game* game) {
	HANDLE hPipe, hEventTemp;
	int i, numClientes = 0;
	DWORD offset, nBytes;

	game->clients.terminar = 0;

	game->event_send_msg = CreateEvent(NULL, FALSE, FALSE, EVENT_CLIENT_NAME);
	if (game->event_send_msg == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o evento"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		CloseHandle(game->sem_write);
		CloseHandle(game->sem_read);
		CloseHandle(game->shut_down_event);
		return FALSE;
	}

	game->event_receive_msg = CreateEvent(NULL, FALSE, FALSE, EVENT_CLIENT_RECEIVE_NAME);
	if (game->event_send_msg == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o evento"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		CloseHandle(game->sem_write);
		CloseHandle(game->sem_read);
		CloseHandle(game->shut_down_event);
		CloseHandle(game->event_send_msg);
		return FALSE;
	}

	for (i = 0; i < N; i++) {

		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
			N,
			256 * sizeof(TCHAR),
			256 * sizeof(TCHAR),
			1000,
			NULL);

		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}

		// criar evento que vai ser associado à esturtura overlaped
		// os eventos aqui tem de ter sempre reset manual e nao automático porque temos de delegar essas responsabilidades ao sistema operativo
		hEventTemp = CreateEvent(NULL, TRUE, FALSE, NULL);

		if (hEventTemp == NULL) {
			_tprintf(TEXT("[ERRO] ao criar evento\n"));
			return -1;
		}

		game->clients.hPipes[i].hPipe = hPipe;
		game->clients.hPipes[i].activo = FALSE;
		ZeroMemory(&game->clients.hPipes[i].overlap, sizeof(game->clients.hPipes[i].overlap));
		game->clients.hPipes[i].overlap.hEvent = hEventTemp;
		game->clients.hEvents[i] = hEventTemp;

		_tprintf(TEXT("[SERVER] Esperar ligação de um cliente... (ConnectNamedPipe)\n"));

		if (ConnectNamedPipe(hPipe, &game->clients.hPipes[i].overlap)) {
			_tprintf(TEXT("[SERVER] Erro ao ligar ao cliente... (ConnectNamedPipe)\n"));
		}
	}

	game->clients.hThread = CreateThread(NULL, 0, clientMsg, game, 0, NULL);
	if (game->clients.hThread == NULL) {
		_tprintf(TEXT("[Erro] ao criar thread!\n"));
		return -1;
	}

	game->clients.loading = -1;

	while (!game->clients.terminar && numClientes < N) {
		offset = WaitForMultipleObjects(N, game->clients.hEvents, FALSE, INFINITE);
		i = offset - WAIT_OBJECT_0;
		DWORD n;
		if (i >= 0 && i < N) {

			_tprintf(TEXT("[SERVER] Cliente %d chegou\n"), i);
			if (GetOverlappedResult(game->clients.hPipes[i].hPipe, &game->clients.hPipes[i].overlap,	
				&nBytes, FALSE)) {
				ResetEvent(game->clients.hEvents[i]);

				WaitForSingleObject(game->mutex, INFINITE);
				game->clients.hPipes[i].activo = TRUE;

				//game->clients.clientThread[i] = CreateThread(NULL, 0, client, game, 0, NULL);

				WriteFile(game->clients.hPipes[i].hPipe, &game->size,
					sizeof(game->size), &n, NULL);
				WriteFile(game->clients.hPipes[i].hPipe, game->world,
					sizeof(game->world), &n, NULL);

				ReleaseMutex(game->mutex);
				game->clients.loading++;
				numClientes++;
			}
		}
	}
}

DWORD WINAPI client(LPVOID param) {
	Game* game = (Game*)param;
	TCHAR buf[256];
	DWORD n, nBytes;
	BOOL ret,ret2;
	int i = game->clients.loading;

	do{
		//WaitForSingleObject(game->event_send_msg, INFINITE);
		ret = ReadFile(game->clients.hPipes[i].hPipe, buf, sizeof(buf) - sizeof(TCHAR), &n,NULL);
	
		buf[n / sizeof(TCHAR)] = '\0';
		_tprintf(TEXT("[Client %d msg] %s\n"), i, buf);
		updatePipe(buf, game);
	} while (1);
}

DWORD WINAPI clientMsg(LPVOID param) {
	Game* game = (Game*)param;
	TCHAR buf[256];
	DWORD n,nBytes;
	BOOL ret;
	int i;
	do {
		WaitForSingleObject(game->event_send_msg,INFINITE);
		for (i = 0; i < N; i++) {
			WaitForSingleObject(game->mutex, INFINITE);
			if (game->clients.hPipes[i].activo) {
				if (GetOverlappedResult(game->clients.hPipes[i].hPipe, &game->clients.hPipes[i].overlap,
					&nBytes, FALSE)) {
					ret = ReadFile(game->clients.hPipes[i].hPipe, buf, sizeof(buf) - sizeof(TCHAR), &n, NULL);
					if (ret && n) {
						buf[n / sizeof(TCHAR)] = '\0';
						_tprintf(TEXT("[Client %d msg] %s\n"), i, buf);
						updatePipe(buf, game);
					}
				}
			}
			ReleaseMutex(game->mutex);
		}
	} while (1);
	game->clients.terminar = 1;

	for (i = 0; i < N; i++)
		SetEvent(game->clients.hEvents[i]);

	return 0;
}