#pragma once

#include <cstdint>

#include "fbxsdk.h"
#pragma comment(lib, "libfbxsdk.lib")


extern "C" namespace FBXLibrary
{
	struct SIMPLE_MATRIX
	{
		union
		{
			struct
			{
				float x[4];
				float y[4];
				float z[4];
				float w[4];
			};

			float values[16];
		};

		float operator[](int i) { return values[i]; }
		const float operator[](int i) const { return values[i]; }
	};

	struct SIMPLE_VERTEX
	{
		float pos[3] = { 0.0f, 0.0f, 0.0f };
		float norm[3] = { 0.0f, 0.0f, 0.0f };
		float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float tex[2] = { 0.0f, 0.0f };

		bool operator==(const SIMPLE_VERTEX rhs)
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

	struct SIMPLE_MESH
	{
		uint32_t			vertex_count = 0;
		uint32_t			index_count = 0;
		SIMPLE_VERTEX*		vertices = nullptr;
		uint32_t*			indices = nullptr;
	};

	struct SIMPLE_MATERIAL
	{
		enum COMPONENT_TYPE { DIFFUSE = 0, EMISSIVE, SPECULAR, NORMALMAP, COUNT };

		struct COMPONENT
		{
			float	value[3] = { 0.0f, 0.0f, 0.0f };
			float	factor = 0.0f;
			int64_t	input = -1;
		};

		COMPONENT& operator[](int i) { return components[i]; }
		const COMPONENT& operator[](int i) const { return components[i]; }

	private:
		COMPONENT components[COUNT];
	};

	struct FBX_JOINT
	{
		FbxNode*	node;
		int			parent_index;
	};

	struct SIMPLE_JOINT
	{
		SIMPLE_MATRIX	global_transform;
		int				parent_index;
	};

	struct SIMPLE_ANIM_FRAME
	{
		double					time;
		std::vector<SIMPLE_MATRIX>	transforms;
	};

	struct SIMPLE_ANIM_CLIP
	{
		double							duration;
		std::vector<SIMPLE_ANIM_FRAME>		frames;
	};


	int CreateAndImport(const char* _fbxFilepath, FbxManager*& _manager, FbxScene*& _scene);

	SIMPLE_MATRIX FbxAMatrixToSimpleMatrix(FbxAMatrix _m);

	int ExtractFbxMesh(const FbxScene* _scene, const char* _outputFilepath, const char* _meshName, int32_t _meshElements);

	int ExtractFbxMaterial(const FbxScene* _scene, const char* _outputFilepath, uint32_t _matNum, int32_t _matElements);

	int ExtractFbxAnimation(const FbxScene* _scene, const char* _outputFilepath);

}
