#include "game.h"
#include "utils.h"

BOOL init_nem_and_sync(Game* game) {
	game->map_file = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM);
	if (game->map_file == NULL) {
		Log((TCHAR*)TEXT("O servidor não está aberto"));
		return FALSE;
	}

	game->shared_mem = (SharedMem*)MapViewOfFile(game->map_file, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(SharedMem));

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

	game->sem_read = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, SEMAPHORE_READ_NAME);

	if (game->sem_read == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o semáforo para leitura"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		return FALSE;
	}

	game->sem_write = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, SEMAPHORE_WRITE_NAME);

	if (game->sem_write == NULL) {
		Log((TCHAR*)_T("Não foi possivel criar o semáforo para Escrita"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		CloseHandle(game->sem_read);
		return FALSE;
	}

	game->command_thread = CreateThread(NULL, 0, command_manager, game, 0, NULL);
	if (game->command_thread == NULL)
	{
		Log((TCHAR*)_T("Não foi possivel criar a thread"));
		UnmapViewOfFile(game->shared_mem);
		CloseHandle(game->map_file);
		CloseHandle(game->mutex);
		CloseHandle(game->event);
		CloseHandle(game->sem_read);
		return FALSE;
	}

	game->shut_down_event = CreateEvent(NULL, FALSE, FALSE, SHUT_DOWN);
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
	return TRUE;
}
