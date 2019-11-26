#pragma once

#include <cstdint>

#include "fbxsdk.h"
#pragma comment(lib, "libfbxsdk.lib")


namespace FBXLibrary
{
	// Joint with FbxNode and parent index
	// Parent index -1 indicates no parent
	struct FBX_JOINT
	{
		FbxNode*	fbx_node_p;
		int			parent_index = -1;
	};


	int CreateAndImport(const char* _fbx_filepath, FbxManager*& _fbx_manager_p, FbxScene*& _scene);

	SIMPLE_MATRIX FbxAMatrixToSimpleMatrix(FbxAMatrix _m);

	int ExtractFbxMesh(const FbxScene* _fbx_scene_p, SIMPLE_MESH& _mesh, const char* _meshName, int32_t _elementOptions);

	int ExtractFbxMaterial(const FbxScene* _fbx_scene_p, SIMPLE_MATERIAL_LIST& _material_list, uint32_t _matNum, int32_t _elementOptions);

	int ExtractFbxAnimation(const FbxScene* _fbx_scene_p, SIMPLE_ANIM_CLIP& _anim_clip, uint32_t _elementOptions);

}
