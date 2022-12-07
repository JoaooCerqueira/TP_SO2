#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

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
#define EVENT_NAME _T("EVENT")
#define SHUT_DOWN _T("SHUTDOWNEVENT")
#define REGISTRY_PATH TEXT("game\\")
#define SEMAPHORE_WRITE_NAME _T("HSEMWRITE")
#define SEMAPHORE_READ_NAME _T("HSEMREAD")
#define AMOUNT_OF_PIPES 6
#define MAX_LENGHT 20
#define MAX 256
#define BUFFERSIZE 10

typedef struct {
	int x;
	int y;
} vect2;

typedef struct {
	TCHAR command[MAX];
} Buffer;

typedef struct {
	TCHAR world[MAX_LENGHT][MAX_LENGHT];
	unsigned int wP;
	unsigned int rP;
	Buffer buffer[BUFFERSIZE];
} SharedMem;

typedef struct {
	vect2 size;
	SharedMem* shared_mem;
	HANDLE mutex;
	HANDLE sem_write;
	HANDLE sem_read;
	HANDLE map_file;
	HANDLE event;
	HANDLE shut_down_event;
	HANDLE command_thread;
	BOOL shutdown;
} Game;

BOOL getDataFromRegistry(Game* game);
void print_world(Game* game);
DWORD WINAPI command_manager(LPVOID p);
BOOL init_nem_and_sync(Game* game);
void closeGame(Game* game);
#endif