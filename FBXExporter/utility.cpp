#include <cstring>

#include "interface.h"

namespace FBXExporter
{
	const char* ReplaceExtension(const char* _filename, const char* _extension)
	{
		int nameLen = strrchr(_filename, '.') - _filename;
		int extLen = strlen(_extension);
		int totalLen = nameLen + extLen + 1;

		char* result = new char[totalLen];

		memset(result, 0, totalLen);
		memcpy(&result[0], _filename, nameLen);
		memcpy(&result[nameLen], _extension, extLen);

		return (const char*)result;
	}
}