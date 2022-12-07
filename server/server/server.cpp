#include "game.h"
#include "utils.h"

void start_game() {
	DWORD option;
	do {
		Log((TCHAR*)_T("Prima 1 para começar o jogo"));
		_tscanf_s(TEXT("%d"), &option, sizeof(DWORD));
	} while (option != 1);
}

int _tmain(int argc, TCHAR* argv[]) {
#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	Game game;
	srand(time(NULL));
	if (!init_game(argc, argv, &game) || !init_nem_and_sync(&game)) {
		exit(-1);
	}

	start_game();
	game.waterThread = CreateThread(NULL, 0, waterMoving, &game, 0, NULL);

	command_handler(&game);

	WaitForSingleObject(game.waterThread, INFINITE);
	closeGame(&game);
}