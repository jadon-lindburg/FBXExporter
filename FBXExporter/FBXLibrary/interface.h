#pragma once

#include <cstdint>

#ifdef FBXLIB_EXPORTS
#define DLLINTERFACE __declspec(dllexport)
#else
#define DLLINTERFACE __declspec(dllimport)
#endif

namespace FBXLibrary
{
	// Mesh elements supported
	enum MeshElement
	{
		Position = 0x00000001
		, Normal = 0x00000002
		, Color = 0x00000004
		, TexCoord = 0x00000008
		, MeshAll = Position | Normal | Color | TexCoord
	};

	// Material elements supported
	enum MaterialElement
	{
		Diffuse = 0x00000001
		, Emissive = 0x00000002
		, Specular = 0x00000004
		, NormalMap = 0x00000008
		, MaterialAll = Emissive | Diffuse | Specular | NormalMap
	};

	// Returns count of polygons in scene
	DLLINTERFACE int GetScenePolyCount(const char* _fbxFilepath);

	// Converts scene mesh for use
	// Use _meshName to search for mesh by name, otherwise first mesh is used by default
	// Use _meshElements to specify which supported elements to store (others default to 0)
	DLLINTERFACE int ExportSimpleMesh(const char* _fbxFilepath, const char* _outputFilepath, const char* _meshName = nullptr, int32_t _meshElements = MeshElement::MeshAll);

	// Returns count of materials in scene
	DLLINTERFACE int GetSceneMaterialCount(const char* _fbxFilepath);

	// Converts scene material for use
	// Use _matElements to specify which supported elements to store (others default to 0)
	DLLINTERFACE int ExportSimpleMaterial(const char* _fbxFilepath, const char* _outputFilepath, uint32_t _matNum = 0, int32_t _matElements = MaterialElement::MaterialAll);
}