#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include "game.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

#define HORIZONTAL_PIPE TEXT("pipeHorizontal.bmp")

void place_images(Client* client, TCHAR matrix[20][20]);

void sync(Client* client) {
	BOOL ret;
	DWORD n;

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	_tprintf(TEXT("[client] Esperar pelo pipe '%s' (WaitNamedPipe)\n"), PIPE_NAME);

	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao servidor\n"));
		exit(-1);
	}

	_tprintf(TEXT("[client] Ligação ao pipe do servidor\n"));

	client->hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (client->hPipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao servidor\n"));
		exit(-1);
	}

	client->event_send_msg = CreateEvent(NULL, FALSE, FALSE, EVENT_CLIENT_NAME);
	if (client->event_send_msg == NULL) {
		exit(-1);
	}

	client->event_receive_msg = CreateEvent(NULL, FALSE, FALSE, EVENT_CLIENT_RECEIVE_NAME);
	if (client->event_receive_msg == NULL) {
		exit(-1);
	}

	// limpamos a struct overlap
	ZeroMemory(
		&client->overlap,
		sizeof(client->overlap)
	);
	// criamos um evento para associar à struct overlap
	client->overlap.hEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL
	);

	_tprintf(TEXT("[client] Liguei-me...\n"));
	TCHAR world[20][20];
	ret = ReadFile(client->hPipe, &client->board_dimesions, sizeof(vect2), &n, NULL);
	ret = ReadFile(client->hPipe, world ,sizeof(world), &n, NULL);

	place_images(client, world);
}

void place_images(Client *client,TCHAR matrix[20][20]) {
	for (int y = 0; y < client->board_dimesions.y; y++){
		for (int x = 0; x < client->board_dimesions.x; x++) {
			if (matrix[y][x] == _T('S')) {
				client->pipes[y][x].image = &client->images[0].start;
			}
			else if (matrix[y][x] == _T('E')) {
				client->pipes[y][x].image = &client->images[0].end;
			}
			else {
				client->pipes[y][x].image = &client->images[0].white_cell;
			}
		}
	}
	
}