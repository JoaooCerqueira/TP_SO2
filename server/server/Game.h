#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "pipes.h"

#include <windows.h>
#include <tchar.h>
#include <math.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h> 
#include <io.h>

#define SHM TEXT("sharedGame")
#define MUTEX_NAME _T("RWMUTEX")
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")
#define EVENT_NAME _T("EVENT")
#define EVENT_CLIENT_NAME _T("EVENT_CLIENT")
#define EVENT_CLIENT_RECEIVE_NAME _T("EVENT_CLIENT_RECEIVE")
#define SHUT_DOWN _T("SHUTDOWNEVENT")
#define REGISTRY_PATH TEXT("game\\")
#define SEMAPHORE_WRITE_NAME _T("HSEMWRITE")
#define SEMAPHORE_READ_NAME _T("HSEMREAD")
#define AMOUNT_OF_PIPES 6
#define MAX_LENGHT 20
#define MAX 256
#define BUFFERSIZE 10

typedef struct {
	TCHAR command[MAX];
} Buffer;

typedef struct {
	int x;
	int y;
} vect2;

typedef struct {
	TCHAR world[MAX_LENGHT][MAX_LENGHT];
	unsigned int wP;
	unsigned int rP;
	Buffer buffer[BUFFERSIZE];
} SharedMem;

typedef struct {
	vect2 size;
	vect2 water_init_pos;
	vect2 water_final_pos;
	SharedMem* shared_mem;
	Clients clients;
	TCHAR world[MAX_LENGHT][MAX_LENGHT];
	DWORD water_time;
	DWORD stop_water;
	TCHAR pipes[AMOUNT_OF_PIPES];
	TCHAR current_piece;
	int current_piece_number;
	vect2 previous_pos;
	vect2 current_pos;
	HANDLE mutex;
	HANDLE sem_write,sem_read;
	HANDLE map_file;
	HANDLE event;
	HANDLE shut_down_event;
	HANDLE waterThread;
	HANDLE event_send_msg,event_receive_msg;
	BOOL random_pipes;
} Game;

BOOL init_game(int argc, TCHAR* argv[], Game* game);
void closeGame(Game* game);
BOOL init_nem_and_sync(Game* game);
void command_handler(Game* game);
void nextPiece(Game* game);
void updatePipe(TCHAR* buf, Game* game);
DWORD WINAPI waterMoving(LPVOID p);
#endif