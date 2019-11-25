#pragma once

#include <cstdint>

#include "fbxsdk.h"
#pragma comment(lib, "libfbxsdk.lib")


namespace FBXLibrary
{
	int CreateAndImport(const char* _fbxFilepath, FbxManager*& _manager, FbxScene*& _scene);

	SIMPLE_MATRIX FbxAMatrixToSimpleMatrix(FbxAMatrix _m);

	int ExtractFbxMesh(const FbxScene* _scene, const char* _outputFilepath, SIMPLE_MESH& _simple_mesh, const char* _meshName, int32_t _meshElements);

	int ExtractFbxMaterial(const FbxScene* _scene, const char* _outputFilepath, SIMPLE_MATERIAL_LIST& _simple_material_list, uint32_t _matNum, int32_t _matElements);

	int ExtractFbxAnimation(const FbxScene* _scene, const char* _outputFilepath, SIMPLE_ANIM_CLIP& _simple_anim_clip, uint32_t _animElements);

}
