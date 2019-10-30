#pragma once

#include "..\FBXLibrary\interface.h"

namespace FBXExporter
{
	const char* ReplaceExtension(const char* _filename, const char* _extension);

	namespace mesh
	{
		int ImportMesh(const char* _inFile, const char* _meshName = nullptr, uint32_t _meshElements = FBXLibrary::MeshElement::MeshPos);
	}

	namespace material
	{
		int ImportMaterials(const char* _filename, int32_t _matElements);
	}
}