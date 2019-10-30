#include "interface.h"

using namespace FBXLibrary;

namespace FBXExporter
{
	namespace material
	{
		// interface
		int ImportMaterials(const char* _filename, int32_t _matElements)
		{
			const char* outFile = ReplaceExtension(_filename, ".mat");

			int result = ExportSimpleMaterial(_filename, outFile, _matElements);

			return result;
		}
	}
}