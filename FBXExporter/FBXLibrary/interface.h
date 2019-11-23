#pragma once

#include <cstdint>

#ifdef FBXLIB_EXPORTS
#define FBXLIB_INTERFACE __declspec(dllexport)
#else
#define FBXLIB_INTERFACE __declspec(dllimport)
#endif

namespace FBXLibrary
{
	// Mesh elements supported
	struct MESH_ELEMENT
	{
		enum
		{
			POSITION = 0x00000001
			, NORMAL = 0x00000002
			, COLOR = 0x00000004
			, TEXCOORD = 0x00000008
			, ALL = POSITION | NORMAL | COLOR | TEXCOORD
		};
	};

	// Material elements supported
	struct MATERIAL_ELEMENT
	{
		enum
		{
			DIFFUSE = 0x00000001
			, EMISSIVE = 0x00000002
			, SPECULAR = 0x00000004
			, NORMALMAP = 0x00000008
			, ALL = EMISSIVE | DIFFUSE | SPECULAR | NORMALMAP
		};
	};

	// Returns count of polygons in scene
	FBXLIB_INTERFACE int GetScenePolyCount(const char* _fbxFilepath);

	// Converts scene mesh for use
	// Use _meshName to search for mesh by name, otherwise first mesh is used by default
	// Use _meshElements to specify which supported elements to store (others default to 0)
	FBXLIB_INTERFACE int ExtractMesh(const char* _fbxFilepath, const char* _outputFilepath, const char* _meshName = nullptr, int32_t _meshElements = MESH_ELEMENT::ALL);

	// Returns count of materials in scene
	FBXLIB_INTERFACE int GetSceneMaterialCount(const char* _fbxFilepath);

	// Converts scene material for use
	// Use _matElements to specify which supported elements to store (others default to 0)
	FBXLIB_INTERFACE int ExtractMaterial(const char* _fbxFilepath, const char* _outputFilepath, uint32_t _matNum = 0, int32_t _matElements = MATERIAL_ELEMENT::ALL);

	// Returns count of poses in scene
	FBXLIB_INTERFACE int GetScenePoseCount(const char* _fbxFilepath);

	// Converts scene animation for use
	// Exports at 30 frames per second
	FBXLIB_INTERFACE int ExtractAnimation(const char* _fbxFilepath, const char* _outputFilepath);
}