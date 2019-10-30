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
		MeshPos			= 0x00000001
		, MeshNorm		= 0x00000002
		, MeshColor		= 0x00000004
		, MeshTex		= 0x00000008
		, MeshAll		= MeshPos | MeshNorm | MeshColor | MeshTex
	};

	// Material elements supported
	enum MaterialElement
	{
		MatDiffuse		= 0x00000001
		, MatEmissive	= 0x00000002
		, MatSpecular	= 0x00000004
		, MatNormalMap	= 0x00000008
		, MatAll		= MatEmissive | MatDiffuse | MatSpecular | MatNormalMap
	};

	// Returns count of polygons in scene
	DLLINTERFACE int GetScenePolyCount(const char* _fbxFilepath);

	// Converts scene mesh for use
	// Use _meshName to search for mesh by name, otherwise first mesh is used by default
	// Use _meshElements to specify which supported elements to store (others default to 0)
	DLLINTERFACE int ExportSimpleMesh(const char* _fbxFilepath, const char* _outputFilepath, const char* _meshName = nullptr, int32_t _meshElements = MeshElement::MeshPos);

	// Returns count of materials in scene
	DLLINTERFACE int GetSceneMaterialCount(const char* _fbxFilepath);

	// Converts scene material for use
	// Use _matElements to specify which supported elements to store (others default to 0)
	DLLINTERFACE int ExportSimpleMaterial(const char* _fbxFilepath, const char* _outputFilepath, uint32_t _matNum = 0, int32_t _matElements = MaterialElement::MatAll);
}