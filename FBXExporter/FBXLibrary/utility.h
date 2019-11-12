#pragma once

#include <cstdint>

#include "fbxsdk.h"
#pragma comment(lib, "libfbxsdk.lib")


extern "C" namespace FBXLibrary
{
	// mesh structs
	struct simple_vertex_s
	{
		float pos[3] = { 0.0f, 0.0f, 0.0f };
		float norm[3] = { 0.0f, 0.0f, 0.0f };
		float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float tex[2] = { 0.0f, 0.0f };

		bool operator==(const simple_vertex_s rhs)
		{
			return
			{
				pos[0] == rhs.pos[0] && pos[1] == rhs.pos[1] && pos[2] == rhs.pos[2]
				&& norm[0] == rhs.norm[0] && norm[1] == rhs.norm[1] && norm[2] == rhs.norm[2]
				&& color[0] == rhs.color[0] && color[1] == rhs.color[1] && color[2] == rhs.color[2] && color[3] == rhs.color[3]
				&& tex[0] == rhs.tex[0] && tex[1] == rhs.tex[1]
			};
		}
	};

	struct simple_mesh_s
	{
		uint32_t			vertexCount = 0;
		uint32_t			indexCount = 0;
		simple_vertex_s*	vertices = nullptr;
		uint32_t*			indices = nullptr;
	};

	// material structs
	struct simple_material_s
	{
		enum eComponent { DIFFUSE = 0, EMISSIVE, SPECULAR, NORMALMAP, COUNT };

		struct component_s
		{
			float	value[3] = { 0.0f, 0.0f, 0.0f };
			float	factor = 0.0f;
			int64_t	input = -1;
		};

		component_s& operator[](int i) { return components[i]; }
		const component_s& operator[](int i) const { return components[i]; }

	private:
		component_s components[COUNT];
	};

	// animation structs
	struct fbx_joint
	{
		FbxNode*	node;
		int			parentIndex;
	};


	FbxManager* CreateAndImport(const char* _fbxFilepath, FbxScene*& _scene);

	int ExtractMesh(const FbxScene* _scene, const char* _outputFilepath, const char* _meshName, int32_t _meshElements);

	int ExtractMaterial(const FbxScene* _scene, const char* _outputFilepath, uint32_t _matNum, int32_t _matElements);

	int ExtractBindPose(const FbxScene* _scene, const char* _outputFilepath);

}
