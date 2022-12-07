#include "utils.h"

void Log(TCHAR* info) {
	_tprintf(TEXT("[server:] %s\n"), info);
}