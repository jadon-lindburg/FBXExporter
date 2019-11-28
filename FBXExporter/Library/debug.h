#ifndef _FBXEXPORTER_LIBRARY_DEBUG_H_
#define _FBXEXPORTER_LIBRARY_DEBUG_H_

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif

#endif // _FBXEXPORTER_LIBRARY_DEBUG_H_