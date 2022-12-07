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

#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")
#define EVENT_NAME _T("EVENT")
#define EVENT_CLIENT_NAME _T("EVENT_CLIENT")
#define EVENT_CLIENT_RECEIVE_NAME _T("EVENT_CLIENT_RECEIVE")

typedef struct {
	BITMAP bitmap;
	HDC DC;
} Image;

typedef struct {
	int x;
	int y;
} vect2;

typedef struct {
	vect2 coord;
	Image *image;
} Pipe;

typedef struct {
	Image horizontal;
	Image vertical;
	Image start;
	Image end;
	Image Left90;
	Image Right90;
	Image Left_1_90;
	Image Right_1_90;
	Image white_cell;
	Image water;
	HANDLE mutex;
	Image barrier;
}Images;

typedef struct {
	HANDLE hPipe;
	OVERLAPPED overlap;
	vect2 board_dimesions, selected_cell;
	Pipe pipes[20][20];
	Images images[2];
	int current_image;
	TCHAR current_pipe;
	TCHAR player[254];
	DWORD hover;
	HWND hWnd, hWnd2;
	HANDLE read;
	HANDLE event_send_msg,event_receive_msg;
} Client;

void sync(Client* client);
void init(Client* client);
void changePipe(Client* client, int y, int x);
void clear_cell(Client* client, int y, int x);
void loadImages(Image* image, HWND hWnd, TCHAR* image_path);
void unpause_water(Client* client,vect2 selected);
void pause_water(Client* client,vect2 selected);
void swap_images(Client* client);
DWORD WINAPI read(LPVOID param);
#endif