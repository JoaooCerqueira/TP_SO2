#include "game.h"
#include "utils.h"
#include "pipes.h"

BOOL top(Game* game);
BOOL bottom(Game* game);
BOOL left(Game* game);
BOOL right(Game* game);
void placeWater(Game* game);

void randomPiece(Game* game) {
	int r = AMOUNT_OF_PIPES % 6;
	game->current_piece = game->pipes[r];
}

void nextPiece(Game* game) {
	game->current_piece_number++;
	if (game->current_piece_number > 5){
		game->current_piece_number = 0;
	}
	game->current_piece = game->pipes[game->current_piece_number];
}

void stop_water(Game* game) {
	if (game->stop_water != 0) {
		_tprintf(TEXT("[server:] A água vai ficar parada por %d\n"), game->stop_water);
		Sleep(game->stop_water * 1E3);
		game->stop_water = 0;
	}
}

DWORD WINAPI waterMoving(LPVOID p) {
	Game* game = (Game*)p;
	DWORD n;

	Sleep(game->water_time * 1E3);
	Log((TCHAR*)TEXT("A agua começou a cair"));

	while (1) {
		stop_water(game);

		if (top(game)) {
			Log((TCHAR*)TEXT("A agua moveu-se para a cima"));
		}
		else if (bottom(game)) {
			Log((TCHAR*)TEXT("A agua moveu-se para a baixo"));
		}
		else if (left(game)) {
			Log((TCHAR*)TEXT("A agua moveu-se para a esquerda"));
		}
		else if (right(game)) {
			Log((TCHAR*)TEXT("A agua moveu-se para a direita"));
		}
		else {
			Log((TCHAR*)TEXT("Não existem mais tubos"));
			Log((TCHAR*)TEXT("Perdes-te"));
			break;
		}
		
		if (game->current_pos.x == game->water_final_pos.x && game->current_pos.y == game->water_final_pos.y) {
			SetEvent(game->event);
			Log((TCHAR*)TEXT("Ganhas-te"));
			break;
		}

		placeWater(game);
		WaitForSingleObject(game->mutex, INFINITE);
	
		CopyMemory(game->shared_mem->world, game->world, sizeof(game->world));
		
		SetEvent(game->event_receive_msg);
		for (int i = 0; i < N; i++) {
			if (game->clients.hPipes[i].activo == 1) {
				WriteFile(game->clients.hPipes[i].hPipe, &game->current_pos,
					sizeof(vect2), &n,NULL);
				WriteFile(game->clients.hPipes[i].hPipe, _T("W"),
					sizeof(TCHAR), &n, NULL);
			}
		}
		ReleaseMutex(game->mutex);

		SetEvent(game->event);
		game->previous_pos.y = game->current_pos.y;
		game->previous_pos.x = game->current_pos.x;
		Sleep(1000);	
	}

	SetEvent(game->event_receive_msg);
	for (int i = 0; i < N; i++) {
		if (game->clients.hPipes[i].activo == 1) {
			WriteFile(game->clients.hPipes[i].hPipe, &game->current_pos,
				sizeof(vect2), &n, NULL);
			WriteFile(game->clients.hPipes[i].hPipe, _T("P"),
				sizeof(TCHAR), &n, NULL);
		}
	}
	_tprintf(_T("Shuting down\n"));
	SetEvent(game->shut_down_event);
	return 1;
}

BOOL top(Game* game) {
	int new_pos_y = game->current_pos.y - 1;
	if (new_pos_y < 0) {
		return FALSE;
	}

	if (new_pos_y == game->previous_pos.y &&
		game->current_pos.x == game->previous_pos.x) {
		return FALSE;
	}

	if (game->world[new_pos_y][game->current_pos.x] == game->pipes[1] ||
		game->world[new_pos_y][game->current_pos.x] == game->pipes[2] ||
		game->world[new_pos_y][game->current_pos.x] == game->pipes[3] ||
		game->world[new_pos_y][game->current_pos.x] == TEXT('E')) {
		game->current_pos.y = new_pos_y;
		return TRUE;
	}
	return FALSE;
}

BOOL bottom(Game* game) {
	int new_pos_y = game->current_pos.y + 1;
	if (new_pos_y > game->size.y) {
		return FALSE;
	}

	if (new_pos_y == game->previous_pos.y &&
		game->current_pos.x == game->previous_pos.x) {
		return FALSE;
	}

	if (game->world[new_pos_y][game->current_pos.x] == game->pipes[1] ||
		game->world[new_pos_y][game->current_pos.x] == game->pipes[4] ||
		game->world[new_pos_y][game->current_pos.x] == game->pipes[5] ||
		game->world[new_pos_y][game->current_pos.x] == TEXT('E')) {
		game->current_pos.y = new_pos_y;
		return TRUE;
	}
	return FALSE;
}

BOOL right(Game* game) {
	int new_pos_x = game->current_pos.x + 1;

	if (new_pos_x > game->size.x) {
		return FALSE;
	}

	if (new_pos_x == game->previous_pos.x &&
		game->current_pos.y == game->previous_pos.y) {
		return FALSE;
	}

	if (game->world[game->current_pos.y][new_pos_x] == game->pipes[0] ||
		game->world[game->current_pos.y][new_pos_x] == game->pipes[4] ||
		game->world[game->current_pos.y][new_pos_x] == game->pipes[3] ||
		game->world[game->current_pos.y][new_pos_x] == TEXT('E')) {
		game->current_pos.x = new_pos_x;
		return TRUE;
	}
	return FALSE;
}

BOOL left(Game* game) {
	int new_pos_x = game->current_pos.x - 1;

	if (new_pos_x < 0) {
		return FALSE;
	}

	if (new_pos_x == game->previous_pos.x &&
		game->current_pos.y == game->previous_pos.y) {
		return FALSE;
	}

	if (game->world[game->current_pos.y][new_pos_x] == game->pipes[0] ||
		game->world[game->current_pos.y][new_pos_x] == game->pipes[2] ||
		game->world[game->current_pos.y][new_pos_x] == game->pipes[5] ||
		game->world[game->current_pos.y][new_pos_x] == TEXT('E')) {
		game->current_pos.x = new_pos_x;
		return TRUE;
	}
	return FALSE;
}

void placeWater(Game* game) {
	game->world[game->current_pos.y][game->current_pos.x] = TEXT('×');
}

void command(Buffer* buffer, Game* game) {
	wchar_t* ptr;
	TCHAR* token = wcstok_s(buffer->command, TEXT(" "), &ptr);
	if (!_tcsicmp(token, TEXT("parar"))) {
		token = wcstok_s(NULL, TEXT(" "), &ptr);
		DWORD timer = _tstoi(token);
		if (timer == 0) {
			Log((TCHAR*)TEXT("Introduza o numero válido"));
		}
		Log((TCHAR*)TEXT("comando parar água recebido"));
		game->stop_water = timer;
		return;
	} else if (!_tcsicmp(token, TEXT("random"))) {
		token = wcstok_s(NULL, TEXT(" "), &ptr);
		if (!_tcsicmp(token, TEXT("sim"))) {
			game->random_pipes = TRUE;
		}
		else if (!_tcsicmp(token, TEXT("não"))) {
			game->random_pipes = FALSE;
		}
		else {
			Log((TCHAR*)TEXT("Introduza sim ou não"));
			return;
		}
		Log((TCHAR*)TEXT("comando random recebido"));
	}
	else if (!_tcsicmp(token, TEXT("parede"))) {
		vect2 coord;
		DWORD n;
		token = wcstok_s(NULL, TEXT(" "), &ptr);
		coord.y = _tstoi(token);
		token = wcstok_s(NULL, TEXT(" "), &ptr);
		coord.x = _tstoi(token);
		Log((TCHAR*)TEXT("comando parede recebido"));
		game->world[coord.y][coord.x] = 'B';

		SetEvent(game->event_receive_msg);
		for (int i = 0; i < N; i++) {
			if (game->clients.hPipes[i].activo == 1) {
				WriteFile(game->clients.hPipes[i].hPipe, &coord,
					sizeof(vect2), &n, &game->clients.hPipes[i].overlap);
				WriteFile(game->clients.hPipes[i].hPipe, _T("B"),
					sizeof(TCHAR), &n, &game->clients.hPipes[i].overlap);
			}
		}
		ResetEvent(game->event_receive_msg);
	}
	else {
		Log((TCHAR*)TEXT("comando inválido"));
	}
}

void command_handler(Game* game) {
	Buffer buffer;
	HANDLE handles[2] = { game->sem_read,game->shut_down_event };
	do {
		DWORD pos = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (pos == 1) {
			break;
		}
		WaitForSingleObject(game->mutex, INFINITE);
		CopyMemory(&buffer, &(game->shared_mem->buffer[(game->shared_mem->rP)++]), sizeof(buffer));
		Log(buffer.command);
		command(&buffer, game);

		if (game->shared_mem->rP == BUFFERSIZE)
			game->shared_mem->rP = 0;

		ReleaseMutex(game->mutex);
		ReleaseSemaphore(game->sem_write, 1, NULL);
	} while (1);
}

void closeGame(Game* game) {
	CloseHandle(game->mutex);
	CloseHandle(game->event);
	UnmapViewOfFile(game->shared_mem);

	for (int i = 0; i < N; i++) {
		_tprintf(TEXT("[ESCRITOR] Desligar o pipe (DisconnectNamedPipe)\n"));

		if (!DisconnectNamedPipe(game->clients.hPipes[i].hPipe)) {
			_tprintf(TEXT("[ERRO] Desligar o pipe! (DisconnectNamedPipe)"));
			exit(-1);
		}
	}

	Log((TCHAR*)TEXT("Todos os handles foram fechados"));
}

void updatePipe(TCHAR* buf, Game* game) {
	wchar_t* ptr;
	TCHAR pipe = buf[_tcslen(buf) - 1];
	DWORD n;

	TCHAR* token = wcstok_s(buf, TEXT(" "), &ptr);
	if (!_tcsicmp(token, TEXT("change"))) {
		token = wcstok_s(NULL, TEXT(" "), &ptr);
		int pos_y = _tstoi(token);
		token = wcstok_s(NULL, TEXT(" "), &ptr);
		int pos_x = _tstoi(token);
		
		vect2 pos;
		BOOL ret;
		DWORD nBytes;
		pos.x = pos_x;
		pos.y = pos_y;
		
		/*
		SetEvent(game->event_receive_msg);
		for (int i = 0; i < N; i++) {
			if (game->clients.hPipes[i].activo) {
				_tprintf(_TEXT("Sending data to client %d"), i);
				WriteFile(game->clients.hPipes[i].hPipe, &pos,
					sizeof(vect2), &n, NULL);

				WriteFile(game->clients.hPipes[i].hPipe,&pipe,
					sizeof(TCHAR), &n, NULL);
			}
		}
		*/

		WaitForSingleObject(game->mutex, INFINITE);
		game->world[pos_y][pos_x] = pipe;
		CopyMemory(game->shared_mem->world, game->world, sizeof(game->world));
		SetEvent(game->event);
		ReleaseMutex(game->mutex);
	}
}
