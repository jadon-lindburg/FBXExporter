#pragma once

#include "..\FBXLibrary\lib_interface.h"


namespace FBXExporter
{
	// Extracts, stores, and optionally exports mesh data from a .fbx file
	int ExtractMeshFromFbxFile(const char* _inFile, const char* _meshName = nullptr, uint32_t _elementOptions = 0, bool _export = false);

	// Extracts, stores, and optionally exports material data from a .fbx file
	int ExtractMaterialsFromFbxFile(const char* _inFile, uint32_t _elementOptions = 0, bool _export = false);

	// Extracts, stores, and optionally exports animation data from a .fbx file
	int ExtractAnimationFromFbxFile(const char* _inFile, uint32_t _elementOptions = 0, bool _export = false);

}
