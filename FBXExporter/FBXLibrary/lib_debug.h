#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif