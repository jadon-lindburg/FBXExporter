#pragma once

#include "..\FBXLibrary\lib_interface.h"


namespace FBXExporter
{
	// Extracts, stores, and optionally exports mesh data from a .fbx file
	int ExtractMesh(const char* _inFile, const char* _meshName = nullptr, uint32_t _meshElements = 0, bool _export = false);

	// Extracts, stores, and optionally exports material data from a .fbx file
	int ExtractMaterials(const char* _inFile, uint32_t _matElements = 0, bool _export = false);

	// Extracts, stores, and optionally exports animation data from a .fbx file
	int ExtractAnimation(const char* _inFile, uint32_t _animElements = 0, bool _export = false);

}
