#include "game.h"

DWORD WINAPI read(LPVOID param) {
	Client* client = (Client*)param;
	vect2 pos;
	TCHAR type;
	DWORD n,nBytes;
	BOOL ret;
	while (1){
		WaitForSingleObject(client->event_receive_msg,INFINITE);
		ReadFile(client->hPipe, &pos, sizeof(pos), &n, NULL);
		ReadFile(client->hPipe, &type, sizeof(TCHAR), &n, NULL);
		
		if (type == _T('P')) {
			CloseHandle(client->read);
			exit(-1);
		}
		else if (type == _T('W')) {
			client->pipes[pos.y][pos.x].image = &client->images[client->current_image].water;
		}
		else if (type == _T('━')) {
			client->pipes[pos.y][pos.x].image = &client->images[client->current_image].horizontal;
		}
		else if (type == _T('┃')) {
			client->pipes[pos.y][pos.x].image = &client->images[client->current_image].vertical;
		}

		else {
			client->pipes[pos.y][pos.x].image = &client->images[client->current_image].barrier;
		}
		InvalidateRect(client->hWnd, NULL, TRUE);
	}
}


void changePipe(Client* client,int y,int x) {
	TCHAR buf[256],value = NULL;
	DWORD n;
	
	if (client->pipes[y][x].image == &client->images[client->current_image].white_cell)
	{
		client->pipes[y][x].image = &client->images[client->current_image].horizontal;
		value = _T('━');
	}
	else if (client->pipes[y][x].image == &client->images[client->current_image].horizontal)
	{
		client->pipes[y][x].image = &client->images[client->current_image].vertical;
		value = _T('┃');
	}
	else if (client->pipes[y][x].image == &client->images[client->current_image].vertical) {
		client->pipes[y][x].image = &client->images[client->current_image].Right90;
		value = _T('┏');
	}
	else if (client->pipes[y][x].image == &client->images[client->current_image].Right90) {
		client->pipes[y][x].image = &client->images[client->current_image].Left90;
		value = _T('┓');
	} 
	else if (client->pipes[y][x].image == &client->images[client->current_image].Left90) {
		client->pipes[y][x].image = &client->images[client->current_image].Left_1_90;
		value = _T('┛');
	}
	else if (client->pipes[y][x].image == &client->images[client->current_image].Left_1_90) {
		client->pipes[y][x].image = &client->images[client->current_image].Right_1_90;
		value = _T('┗');
	}
	else if (client->pipes[y][x].image == &client->images[client->current_image].Right_1_90) {
		client->pipes[y][x].image = &client->images[client->current_image].white_cell;
		value = _T('-');
	}
	
	_stprintf_s(buf, _T("change %d %d %c"), y, x,value);
		
	BOOL ret;
	DWORD nBytes;

	SetEvent(client->event_send_msg);
	WriteFile(client->hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n,NULL);
}

void clear_cell(Client* client, int y, int x) {
	client->pipes[y][x].image = &client->images[client->current_image].white_cell;
}

void pause_water(Client* client,vect2 selected) {
	TCHAR buf[256];
	DWORD n;

	_stprintf_s(buf, _T("pause %d %d"), selected.y, selected.x);

	WriteFile(client->hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, &client->overlap);
	SetEvent(client->event_send_msg);
}

void unpause_water(Client* client, vect2 selected) {
	TCHAR buf[256];
	DWORD n;

	_stprintf_s(buf, _T("unpause %d %d"), selected.y, selected.x);

	WriteFile(client->hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
	SetEvent(client->event_send_msg);
}

void swap_images(Client *client) {
	int old = client->current_image;
	client->current_image == 1 ? client->current_image = 0 : client->current_image = 1;

	for (int y = 0; y < client->board_dimesions.y; y++) {
		for (int x = 0; x < client->board_dimesions.x; x++) {
			if (client->pipes[y][x].image == &client->images[old].white_cell)
			{
				client->pipes[y][x].image = &client->images[client->current_image].white_cell;
			}
			else if (client->pipes[y][x].image == &client->images[old].horizontal)
			{
				client->pipes[y][x].image = &client->images[client->current_image].horizontal;
			}
			else if (client->pipes[y][x].image == &client->images[old].vertical) {
				client->pipes[y][x].image = &client->images[client->current_image].vertical;
			}
			else if (client->pipes[y][x].image == &client->images[old].Right90) {
				client->pipes[y][x].image = &client->images[client->current_image].Right90;
			}
			else if (client->pipes[y][x].image == &client->images[old].Left90) {
				client->pipes[y][x].image = &client->images[client->current_image].Left90;
			}
			else if (client->pipes[y][x].image == &client->images[old].Left_1_90) {
				client->pipes[y][x].image = &client->images[client->current_image].Left_1_90;
			}
			else if (client->pipes[y][x].image == &client->images[old].Right_1_90) {
				client->pipes[y][x].image = &client->images[client->current_image].Right_1_90;
			}
			else if (client->pipes[y][x].image == &client->images[old].water) {
				client->pipes[y][x].image = &client->images[client->current_image].water;
			}
			else if (client->pipes[y][x].image == &client->images[old].barrier) {
				client->pipes[y][x].image = &client->images[client->current_image].barrier;
			}
			else if (client->pipes[y][x].image == &client->images[old].start) {
				client->pipes[y][x].image = &client->images[client->current_image].start;
			}
			else if (client->pipes[y][x].image == &client->images[old].end) {
				client->pipes[y][x].image = &client->images[client->current_image].end;
			}
		}
	}
}
