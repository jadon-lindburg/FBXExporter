#pragma once

#include "..\FBXLibrary\interface.h"


namespace FBXExporter
{
	namespace Mesh
	{
		int ExportMesh(const char* _inFile, const char* _meshName = nullptr, uint32_t _meshElements = FBXLibrary::MeshElement::MeshPos);
	}

	namespace Material
	{
		int ExportMaterials(const char* _filename, int32_t _matElements);
	}

}
