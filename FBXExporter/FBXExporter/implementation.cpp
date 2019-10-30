#include <cstring>

#include "interface.h"
#include "utility.h"


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


	namespace Mesh
	{
		// interface
		int ExportMesh(const char* _filename, const char* _meshName, uint32_t _meshElements)
		{
			const char* outFile = ReplaceExtension(_filename, ".mesh");

			int result = FBXLibrary::ExportSimpleMesh(_filename, outFile, nullptr, _meshElements);

			return result;
		}

	}

	namespace Material
	{
		// interface
		int ExportMaterials(const char* _filename, int32_t _matElements)
		{
			const char* outFile = ReplaceExtension(_filename, ".mat");

			int result = FBXLibrary::ExportSimpleMaterial(_filename, outFile, _matElements);

			return result;
		}

	}

}
