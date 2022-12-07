#include "game.h"
#include "utils.h"
#define MAX 256

BOOL getDataFromRegistry(Game* game);
BOOL getDataFromCommandLine(Game* game, TCHAR* argv[]);
void initPipes(Game* game);
void initBoard(Game* game);
void randomWaterPosition(Game* game);

BOOL init_game(int argc, TCHAR* argv[], Game* game) {
	if (argc == 4 && !getDataFromCommandLine(game, argv)) {
		return FALSE;
	}
	else if (!getDataFromRegistry(game)) {
		return FALSE;
	}

	initPipes(game);
	initBoard(game);
	randomWaterPosition(game);

	game->stop_water = 0;
	game->random_pipes = FALSE;
	game->current_piece_number = 0;

	Log((TCHAR*)TEXT("Jogo inicializado"));
}

BOOL getDataFromCommandLine(Game* game, TCHAR* argv[]) {
	HKEY hkey;
	game->size.x = _tstoi(argv[1]);
	game->size.y = _tstoi(argv[2]);
	game->water_time = _tstoi(argv[3]);

	DWORD dontcare;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("game\\\0"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &dontcare) != ERROR_SUCCESS) {
		Log((TCHAR*)TEXT("Chave já esta criada"));
	}
	RegSetValueEx(hkey, TEXT("x"), 0, REG_SZ, (LPBYTE)argv[1], (DWORD)(_tcslen(argv[1]) + 1) * sizeof(TCHAR));
	RegSetValueEx(hkey, TEXT("y"), 0, REG_SZ, (LPBYTE)argv[2], (DWORD)(_tcslen(argv[2]) + 1) * sizeof(TCHAR));
	RegSetValueEx(hkey, TEXT("w"), 0, REG_SZ, (LPBYTE)argv[2], (DWORD)(_tcslen(argv[3]) + 1) * sizeof(TCHAR));

	if (game->size.x == 0 || game->size.y == 0 || game->water_time == 0) {
		Log((TCHAR*)TEXT("Deve introduzir [Linhas] [Colunas] [Tempo da água]"));
		return FALSE;
	}

	Log((TCHAR*)TEXT("Chave criadas"));
	return TRUE;
}

BOOL getDataFromRegistry(Game* game) {
	HKEY hkey;
	TCHAR value[MAX];
	DWORD sizeval = 257 * sizeof(DWORD);

	if (RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH, 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
		Log((TCHAR*)TEXT("Não existe essa chave"));
		return FALSE;
	}

	if (RegQueryValueEx(hkey, TEXT("x\0"), NULL, NULL, (LPBYTE)value, &sizeval) != ERROR_SUCCESS) {
		Log((TCHAR*)TEXT("Não existe o valor para [x]"));
		return FALSE;
	}

	game->size.x = _tstoi(value);
	if (RegQueryValueEx(hkey, TEXT("y\0"), NULL, NULL, (LPBYTE)value, &sizeval) != ERROR_SUCCESS) {
		Log((TCHAR*)TEXT("Não existe o valor para [y]"));
		return FALSE;
	}
	game->size.y = _tstoi(value);

	if (RegQueryValueEx(hkey, TEXT("w\0"), NULL, NULL, (LPBYTE)value, &sizeval) != ERROR_SUCCESS) {
		Log((TCHAR*)TEXT("Não existe o valor para [water]"));
		return FALSE;
	}
	game->water_time = _tstoi(value);
	return TRUE;
}

void initPipes(Game* game) {
	game->pipes[0] = TEXT('━');
	game->pipes[1] = TEXT('┃');
	game->pipes[2] = TEXT('┏');
	game->pipes[3] = TEXT('┓');
	game->pipes[4] = TEXT('┛');
	game->pipes[5] = TEXT('┗');

	game->current_piece = game->pipes[0];
}

void initBoard(Game* game) {
	for (DWORD y = 0; y < game->size.y; y++) {
		for (DWORD x = 0; x < game->size.x; x++) {
			game->world[y][x] = _T('-');
		}
	}
}

void randomWaterPosition(Game* game) {
	game->water_init_pos.x = 0;
	game->water_init_pos.y = 0;
	game->water_final_pos.x = 0;
	game->water_final_pos.y = 2;

	game->world[game->water_init_pos.y][game->water_init_pos.x] = _T('S');
	game->world[game->water_final_pos.y][game->water_final_pos.x] = _T('E');

	game->current_pos.x = game->water_init_pos.x;
	game->current_pos.y = game->water_init_pos.y;
}