#include "game.h"
#define MAX 256

BOOL getDataFromRegistry(Game* game) {
	HKEY hkey;
	TCHAR value[MAX];
	DWORD sizeval = 257 * sizeof(value);

	if (RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH, 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
		_tprintf(TEXT("Nao exite esse registo\n"));
		return FALSE;
	}

	if (RegQueryValueEx(hkey, TEXT("x\0"), NULL, NULL, (LPBYTE)value, &sizeval) != ERROR_SUCCESS) {
		_tprintf(TEXT("Nao exite o valor [x]\n"));
		return FALSE;
	}

	game->size.x = _tstoi(value);
	if (RegQueryValueEx(hkey, TEXT("y\0"), NULL, NULL, (LPBYTE)value, &sizeval) != ERROR_SUCCESS) {
		_tprintf(TEXT("Nao exite o valor [y]\n"));
		return FALSE;
	}
	game->size.y = _tstoi(value);

	return TRUE;
}