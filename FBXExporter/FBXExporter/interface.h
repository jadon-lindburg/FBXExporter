#pragma once

#include "..\FBXLibrary\interface.h"


namespace FBXExporter
{
	namespace Mesh
	{
		int ExportMesh(const char* _inFile, const char* _meshName = nullptr, uint32_t _meshElements = FBXLibrary::MESH_ELEMENT::POSITION);
	}

	namespace Material
	{
		int ExportMaterials(const char* _inFile, int32_t _matElements);
	}

	namespace Animation
	{
		int ExportAnimation(const char* _inFile);
	}

}
