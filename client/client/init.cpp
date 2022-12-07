#include "game.h"

void init(Client* client) {
	client->hover = 0;
	client->current_image = 0;

	int pos_y = 100;
	int pos_x = 100;
	for (int y = 0; y < client->board_dimesions.y; y++) {
		for (int x = 0; x < client->board_dimesions.x; x++) {
			client->pipes[y][x].coord.y = pos_y;
			client->pipes[y][x].coord.x = pos_x;
			pos_x += 100;
		}
		pos_x = 100;
		pos_y += 100;
	}
}

void loadImages(Image* image, HWND hWnd,TCHAR *image_path) {
	HDC hdc;
	HANDLE hBmp = (HBITMAP)LoadImage(NULL, image_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hBmp, sizeof(image->bitmap), &image->bitmap);
	hdc = GetDC(hWnd);
	image->DC = CreateCompatibleDC(hdc);
	SelectObject(image->DC, hBmp);
	ReleaseDC(hWnd, hdc);
}