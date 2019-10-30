#include "interface.h"

using namespace FBXLibrary;

namespace FBXExporter
{
	namespace mesh
	{
		// interface
		int ImportMesh(const char* _filename, const char* _meshName, uint32_t _meshElements)
		{
			const char* outFile = ReplaceExtension(_filename, ".mesh");

			int result = ExportSimpleMesh(_filename, outFile, nullptr, _meshElements);

			return result;
		}
	}
}