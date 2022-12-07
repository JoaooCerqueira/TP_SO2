#include <windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "game.h"
#include "resource.h"
#include "utils.h"

#define HORIZONTAL_PIPE TEXT("pipes/horizontal.bmp")
#define VERTICAL_PIPE TEXT("pipes/vertical.bmp")
#define START_PIPE TEXT("pipes/start.bmp")
#define WHITE_CELL TEXT("whiteCell.bmp")
#define END_PIPE TEXT("pipes/end.bmp")
#define LEFT90 TEXT("pipes/90Left.bmp")
#define RIGHT90 TEXT("pipes/90Right.bmp")
#define RIGHT90_1 TEXT("pipes/90_1Left.bmp")
#define LEFT90_1 TEXT("pipes/90_1Right.bmp")
#define WATER TEXT("pipes/water.bmp")
#define BARRIER TEXT("pipes/barrier.bmp")

#define HORIZONTAL_PIPE_2 TEXT("pipes2/horizontal.bmp")
#define VERTICAL_PIPE_2 TEXT("pipes2/vertical.bmp")
#define START_PIPE_2 TEXT("pipes2/start.bmp")
#define WHITE_CELL_2 TEXT("whiteCell.bmp")
#define END_PIPE_2 TEXT("pipes2/end.bmp")
#define LEFT90_2 TEXT("pipes2/90Left.bmp")
#define RIGHT90_2 TEXT("pipes2/90Right.bmp")
#define RIGHT90_1_2 TEXT("pipes2/90_1Left.bmp")
#define LEFT90_1_2 TEXT("pipes2/90_1Right.bmp")
#define WATER_2 TEXT("pipes2/water.bmp")
#define BARRIER_2 TEXT("pipes2/barrier.bmp")



LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Login(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

TCHAR szProgName[] = TEXT("Client");
HINSTANCE hInstance;

HWND CriarJanela(HINSTANCE hThisInst, TCHAR* szWinName,int width,int height) {
	return CreateWindow(
		szWinName,			// Nome da janela (programa) definido acima
		TEXT("SO2 - Client"),// Texto que figura na barra do t tulo
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,	// Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT,		// Posi  o x pixels (default=  direita da  ltima)
		CW_USEDEFAULT,		// Posi  o y pixels (default=abaixo da  ltima)
		width,		// Largura da janela (em pixels)
		height,		// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		hThisInst,		// handle da inst ncia do programa actual ("hInst"   
		// passado num dos par metros de WinMain()
		0);
}

ATOM RegistaClasse(HINSTANCE hThisInst, TCHAR* szWinName,WNDPROC func) {
	WNDCLASSEX wcl;
	wcl.cbSize = sizeof(WNDCLASSEX);
	wcl.hInstance = hThisInst; // handle para esta instância
	wcl.lpszClassName = szWinName; // nome da classe
	wcl.lpfnWndProc = func; // função da janela
	wcl.style = CS_HREDRAW; // estilo default+
	// CS_GLOBALCLASS -> classe global em vez de local
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // icon std
	wcl.hIconSm = LoadIcon(NULL, IDI_WINLOGO); // icon menor
	// wcl.hIconSm = LoadIcon(hThisInst, (LPCTSTR)IDI_ICON1);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW); // cur. style
	wcl.lpszMenuName = MAKEINTRESOURCE(IDR_MENU3); // sem menu
	wcl.cbClsExtra = 0; // sem info extra
	wcl.cbWndExtra = sizeof(Client*); // sem info extra
	wcl.hbrBackground = CreateSolidBrush(RGB(155, 155, 155));
	// cor de fundo = branco
	return RegisterClassEx(&wcl);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	Client client;

	HWND hWnd,hWnd2;
	MSG lpMsg;
	WNDCLASSEX wcApp;

	//Janela 1
	if (!RegistaClasse(hInst,(TCHAR*) _T("SO2 - Game"),TrataEventos))
		return 0;

	hWnd = CriarJanela(hInst,(TCHAR*)_T("SO2 - Game"),1000,700);
	client.hWnd = hWnd;

	if (!RegistaClasse(hInst, (TCHAR*)_T("S"),Login))
		return 0;

	hWnd2 = CriarJanela(hInst, (TCHAR*)_T("S"),500, 500);

	loadImages(&client.images[0].horizontal, hWnd, (TCHAR*)HORIZONTAL_PIPE);
	loadImages(&client.images[0].start, hWnd, (TCHAR*)START_PIPE);
	loadImages(&client.images[0].end, hWnd, (TCHAR*)END_PIPE);
	loadImages(&client.images[0].vertical, hWnd, (TCHAR*)VERTICAL_PIPE);
	loadImages(&client.images[0].Right90, hWnd, (TCHAR*)RIGHT90);
	loadImages(&client.images[0].Left90, hWnd, (TCHAR*)LEFT90);
	loadImages(&client.images[0].Left_1_90, hWnd, (TCHAR*)LEFT90_1);
	loadImages(&client.images[0].Right_1_90, hWnd, (TCHAR*)RIGHT90_1);
	loadImages(&client.images[0].white_cell, hWnd, (TCHAR*)WHITE_CELL);
	loadImages(&client.images[0].water, hWnd, (TCHAR*)WATER);
	loadImages(&client.images[0].barrier, hWnd, (TCHAR*)BARRIER);

	loadImages(&client.images[1].horizontal, hWnd, (TCHAR*)HORIZONTAL_PIPE_2);
	loadImages(&client.images[1].start, hWnd, (TCHAR*)START_PIPE_2);
	loadImages(&client.images[1].end, hWnd, (TCHAR*)END_PIPE_2);
	loadImages(&client.images[1].vertical, hWnd, (TCHAR*)VERTICAL_PIPE_2);
	loadImages(&client.images[1].Right90, hWnd, (TCHAR*)RIGHT90_2);
	loadImages(&client.images[1].Left90, hWnd, (TCHAR*)LEFT90_2);
	loadImages(&client.images[1].Left_1_90, hWnd, (TCHAR*)LEFT90_1_2);
	loadImages(&client.images[1].Right_1_90, hWnd, (TCHAR*)RIGHT90_1_2);
	loadImages(&client.images[1].white_cell, hWnd, (TCHAR*)WHITE_CELL_2);
	loadImages(&client.images[1].water, hWnd, (TCHAR*)WATER_2);
	loadImages(&client.images[1].barrier, hWnd, (TCHAR*)BARRIER_2);

	sync(&client);
	init(&client);
	
	client.read = CreateThread(NULL, 0, read, &client, 0, NULL);
	if (client.read == NULL) {
		_tprintf(TEXT("[Erro] ao criar thread!\n"));
		return -1;
	}

	SetWindowLongPtr(hWnd, 0, (LONG_PTR) &client);
	SetWindowLongPtr(hWnd2, 0, (LONG_PTR)&client);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	ShowWindow(hWnd2, nCmdShow);
	UpdateWindow(hWnd2);

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}

	return((int)lpMsg.wParam);
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	Client* client;
	vect2 pos;

	BOOL fMouseTracking = FALSE;
	HDC hdc;
	RECT rect;
	int msgboxID; 
	MINMAXINFO* mmi;
	PAINTSTRUCT ps;
	static HDC horizontal = NULL;

	HBITMAP hBmp = NULL;
	static BITMAP bmp;

	switch (messg) {
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		break;
	case WM_CLOSE:
		msgboxID = MessageBox(hWnd, TEXT("Deseja sair?"), TEXT("Confirmar"), MB_YESNO | MB_ICONQUESTION);
		if (msgboxID == IDYES) {
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY:
	
		PostQuitMessage(0);	
		break;
	case WM_COMMAND:
		client = (Client*)GetWindowLongPtr(hWnd, 0);

		switch (LOWORD(wParam))
		{
		case 0:
			swap_images(client);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	case WM_CHAR:
		break;

	case WM_LBUTTONDOWN:
		client = (Client*)GetWindowLongPtr(hWnd, 0);

		pos.x = GET_X_LPARAM(lParam);
		pos.y = GET_Y_LPARAM(lParam);

		for (int y = 0; y < client->board_dimesions.y; y++) {
			for (int x = 0; x < client->board_dimesions.x; x++) {
				if (pos.x > client->pipes[y][x].coord.x && pos.x < client->pipes[y][x].coord.x + client->pipes[y][x].image->bitmap.bmWidth &&
					pos.y > client->pipes[y][x].coord.y && pos.y < client->pipes[y][x].coord.y + client->pipes[y][x].image->bitmap.bmHeight) {
					changePipe(client,y,x);
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
		}
		break;

	case WM_RBUTTONDOWN:
		client = (Client*)GetWindowLongPtr(hWnd, 0);

		pos.x = GET_X_LPARAM(lParam);
		pos.y = GET_Y_LPARAM(lParam);

		for (int y = 0; y < client->board_dimesions.y; y++) {
			for (int x = 0; x < client->board_dimesions.x; x++) {
				if (pos.x > client->pipes[y][x].coord.x && pos.x < client->pipes[y][x].coord.x + client->pipes[y][x].image->bitmap.bmWidth &&
					pos.y > client->pipes[y][x].coord.y && pos.y < client->pipes[y][x].coord.y + client->pipes[y][x].image->bitmap.bmHeight) {
					clear_cell (client, y, x);
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
		}
		break;

	case WM_MOUSEMOVE:
		client = (Client*)GetWindowLongPtr(hWnd, 0);
		pos.x = GET_X_LPARAM(lParam);
		pos.y = GET_Y_LPARAM(lParam);

		for (int y = 0; y < client->board_dimesions.y; y++) {
			for (int x = 0; x < client->board_dimesions.x; x++) {
				if (!fMouseTracking && pos.x > client->pipes[y][x].coord.x && pos.x < client->pipes[y][x].coord.x + client->pipes[y][x].image->bitmap.bmWidth &&
					pos.y > client->pipes[y][x].coord.y && pos.y < client->pipes[y][x].coord.y + client->pipes[y][x].image->bitmap.bmHeight)
				{
					client->selected_cell.y = y;
					client->selected_cell.x = x;

					TRACKMOUSEEVENT tme;

					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_HOVER;
					tme.hwndTrack = hWnd;
					tme.dwHoverTime = 2 * 1E3;
					fMouseTracking = TrackMouseEvent(&tme);
				}
			}
		} 
		break;
	case WM_MOUSEHOVER:
		client = (Client*)GetWindowLongPtr(hWnd, 0);

		fMouseTracking = FALSE;
		client->hover++;
		if (client->hover <= 3) {
			pause_water(client, client->selected_cell);
		}
		break;
	case WM_MOUSELEAVE:
		client = (Client*)GetWindowLongPtr(hWnd, 0);
		fMouseTracking = FALSE;

		unpause_water(client, client->selected_cell);
		break;
	case WM_PAINT:
		client = (Client*)GetWindowLongPtr(hWnd, 0);
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, CreateSolidBrush(RGB(0,0,0)));
		
		for (int y = 0; y < client->board_dimesions.y; y++) {
			for (int x = 0; x < client->board_dimesions.x; x++) {
				BitBlt(hdc, client->pipes[y][x].coord.x,client->pipes[y][x].coord.y, client->pipes[y][x].image->bitmap.bmWidth, 
					client->pipes[y][x].image->bitmap.bmHeight, client->pipes[y][x].image->DC, 0, 0, SRCCOPY);
			}
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_ERASEBKGND:
		return 1;

	case WM_GETMINMAXINFO:
		mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = bmp.bmWidth + 2;
		mmi->ptMinTrackSize.y = bmp.bmHeight * 2;
		break;

	case WM_SIZE:
		break;
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;  // break tecnicamente desnecess rio por causa do return
	}
	return(0);
}

LRESULT CALLBACK Login(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	Client* client;
	
	HDC hdc;
	RECT rect;
	int msgboxID;
	MINMAXINFO* mmi;
	PAINTSTRUCT ps;
	
	HWND button;
	static HWND edit;


	switch (messg) {
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		edit = CreateWindow(
			TEXT("EDIT"),
			NULL,
			WS_VISIBLE | WS_CHILD | SS_CENTER | WS_CLIPCHILDREN,
			0,100, 255, 50,
			hWnd,
			NULL,
			hInstance,
			NULL
		);

		button = CreateWindow(TEXT("button"), TEXT("player name"), WS_VISIBLE | WS_CHILD,
			0, 150, 255, 50, hWnd, (HMENU)103, NULL, NULL);
		break;
	case WM_COMMAND:
		client = (Client*)GetWindowLongPtr(hWnd, 0);

		if (HIWORD(wParam) == BN_CLICKED) {


			int len = GetWindowTextLength(edit) + 1;

			GetWindowText(edit, client->player, len);
			SetWindowText(hWnd, client->player);
			ShowWindow(hWnd, SW_HIDE);
		}

		break;
	case WM_CLOSE:
		msgboxID = MessageBox(hWnd, TEXT("Deseja sair?"), TEXT("Confirmar"), MB_YESNO | MB_ICONQUESTION);
		if (msgboxID == IDYES) {
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY:
		// "PostQuitMessage(Exit Status)"
		PostQuitMessage(0);
		break;

	case WM_CHAR:			//TECLAS;
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		break;
	case WM_MOUSEHOVER:
		break;
	case WM_MOUSELEAVE:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, CreateSolidBrush(RGB(155, 155, 155)));
		EndPaint(hWnd, &ps);
		break;

	case WM_ERASEBKGND:
		return 1;

	case WM_GETMINMAXINFO:
		break;

	case WM_SIZE:
		break;
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;  // break tecnicamente desnecess rio por causa do return
	}
	return(0);
}