#include "game.h"
#include "utils.h"

int _tmain(int argc, TCHAR* argv[]) {
	srand(time(NULL));
#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	Game game;
	HANDLE print;
	if (!init_nem_and_sync(&game)) exit(-1);
	getDataFromRegistry(&game);

	print_world(&game);

	Log((TCHAR*) TEXT("Escreve algo para fechar"));
	WaitForSingleObject(game.command_thread, INFINITE);

	closeGame(&game);
	return 1;
}