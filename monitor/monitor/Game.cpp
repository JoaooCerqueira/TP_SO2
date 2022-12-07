#include "game.h"
#include "utils.h"

void print_world(Game* game) {
	HANDLE handles[2] = { game->shut_down_event,game->event};
	while (1) {
		DWORD pos = WaitForMultipleObjects(2,handles, FALSE, INFINITE);
		if (pos == 0) {
			break;
		}
		for (int y = 0; y < game->size.y; y++) {
			for (int x = 0; x < game->size.x; x++) {
				_tprintf(TEXT(" %c "), game->shared_mem->world[y][x]);
			}
			_tprintf(TEXT("\n"));
		}
		for (int i = 0; i < game->size.x * 3; i++) {
			_tprintf(TEXT("━"));
		}
		_tprintf(TEXT("\n"));
	}
	_tprintf(_TEXT("Shuting down\n"));
}

DWORD WINAPI command_manager(LPVOID p) {
	Game* game = (Game*)p;
	Buffer buffer;
	HANDLE handles[2] = { game->shut_down_event,game->sem_write};
	if (!game->shutdown)
		return 0;

	while (1) {
		Log((TCHAR*)TEXT("Introduza um comando"));
		_fgetts(buffer.command,sizeof(buffer.command),stdin);
		DWORD pos = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (pos == 0) {
			break;
		}
		WaitForSingleObject(game->mutex, INFINITE);
		CopyMemory(&(game->shared_mem->buffer[(game->shared_mem->wP)++]), &buffer, sizeof(buffer));

		if ((game->shared_mem->wP) == BUFFERSIZE)
			(game->shared_mem->wP) = 0;	

		ReleaseMutex(game->mutex);
		ReleaseSemaphore(game->sem_read, 1, NULL);
		Sleep(2 * 1E3);
	}
	return 0;
}

void closeGame(Game* game) {
	CloseHandle(game->mutex);
	CloseHandle(game->event);
	UnmapViewOfFile(game->shared_mem);
	_tprintf(TEXT("Todos os handles foram fechados\n"));;
}