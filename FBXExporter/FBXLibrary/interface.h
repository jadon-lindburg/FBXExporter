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
	FBXLIB_INTERFACE int GetScenePolyCount(const char* _fbxFilepath);

	// Converts scene mesh for use
	// Use _meshName to search for mesh by name, otherwise first mesh is used by default
	// Use _meshElements to specify which supported elements to store (others default to 0)
	FBXLIB_INTERFACE int ExportSimpleMesh(const char* _fbxFilepath, const char* _outputFilepath, const char* _meshName = nullptr, int32_t _meshElements = MeshElement::MeshAll);

	// Returns count of materials in scene
	FBXLIB_INTERFACE int GetSceneMaterialCount(const char* _fbxFilepath);

	// Converts scene material for use
	// Use _matElements to specify which supported elements to store (others default to 0)
	FBXLIB_INTERFACE int ExportSimpleMaterial(const char* _fbxFilepath, const char* _outputFilepath, uint32_t _matNum = 0, int32_t _matElements = MaterialElement::MaterialAll);

	// Returns count of poses in scene
	FBXLIB_INTERFACE int GetScenePoseCount(const char* _fbxFilepath);

	// Converts scene bind pose for use
	FBXLIB_INTERFACE int ExportBindPose(const char* _fbxFilepath, const char* _outputFilepath);
}