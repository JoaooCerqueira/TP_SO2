#ifndef PIPES_H_INCLUDED
#define PIPES_H_INCLUDED

#include <windows.h>

#define N 1

// estrutura do named pipe
typedef struct {
	HANDLE hPipe; // handle do pipe
	OVERLAPPED overlap;
	BOOL activo; //representa se a instancia do named pipe esta ou nao ativa, se ja tem um cliente ou nao

} PipeData;

typedef struct {
	PipeData hPipes[N];
	HANDLE hEvents[N];
	HANDLE hThread;
	HANDLE clientThread[N];
	int loading;
	int terminar;
} Clients;
#endif