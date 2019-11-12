#include <cstring>

#include "interface.h"
#include "utility.h"


namespace FBXExporter
{
	const char* ReplaceExtension(const char* _filename, const char* _extension)
	{
		int nameLen = (int)(strrchr(_filename, '.') - _filename);
		int extLen = (int)strlen(_extension);
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
		int ExportMesh(const char* _inFile, const char* _meshName, uint32_t _meshElements)
		{
			const char* outFile = ReplaceExtension(_inFile, ".mesh");

			int result = FBXLibrary::ExportSimpleMesh(_inFile, outFile, nullptr, _meshElements);

			return result;
		}

	}

	namespace Material
	{
		// interface
		int ExportMaterials(const char* _inFile, int32_t _matElements)
		{
			const char* outFile = ReplaceExtension(_inFile, ".mat");

			int result = FBXLibrary::ExportSimpleMaterial(_inFile, outFile, _matElements);

			return result;
		}

	}

	namespace Animation
	{
		// interface
		int ExportBindPose(const char* _inFile)
		{
			const char* outFile = ReplaceExtension(_inFile, ".bindpose");

			int result = FBXLibrary::ExportBindPose(_inFile, outFile);

			return result;
		}

	}

}
