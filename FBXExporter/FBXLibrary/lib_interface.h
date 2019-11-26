#pragma once

#include <array>
#include <cstdint>
#include <vector>


#ifdef FBXLIB_EXPORTS
#define FBXLIB_INTERFACE __declspec(dllexport)
#else
#define FBXLIB_INTERFACE __declspec(dllimport)
#endif


namespace FBXLibrary
{
#pragma region Defines
	// macros used to specify the result of functions and operations
#define FAIL -1
#define SUCCESS 0
#define EXTRACT 1
#define EXPORT 2

	// macros used to test the result of functions and operations
#define FAILED(r) (r < 0)
#define SUCCEEDED(r) (r >= 0)

	// char array used to store filepaths instead of char* because the inconsistent length of char* would make writing and reading files more complicated
	using filepath_t = std::array<char, 260>;
#pragma endregion

#pragma region Options Structs
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

	// Animation elements supported
	struct ANIMATION_ELEMENT
	{
		enum
		{
		};
	};
#pragma endregion

#pragma region Data Structs
	// 4x4 float matrix
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

	// Vertex with float3 position, float3 normal, float4 color, and float2 texture coordinate
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

	// Mesh with vertex count, index count, list of vertices, and list of indices
	struct SIMPLE_MESH
	{
		uint32_t						vertex_count = 0;
		uint32_t						index_count = 0;
		std::vector<SIMPLE_VERTEX>		vertices;
		std::vector<uint32_t>			indices;
	};

	// Material with diffuse, emissive, specular, and normal map components
	// Components contain a value, factor, and filepath index
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

	// List of materials and filepaths
	struct SIMPLE_MATERIAL_LIST
	{
		std::vector<SIMPLE_MATERIAL>	materials;
		std::vector<filepath_t>			filepaths;
	};

	// Joint with matrix transform and parent index
	// Parent index -1 indicates no parent
	struct SIMPLE_JOINT
	{
		SIMPLE_MATRIX	global_transform;
		int				parent_index = -1;
	};

	// Animation frame with keytime and list of joint transforms
	struct SIMPLE_ANIM_FRAME
	{
		double						time;
		std::vector<SIMPLE_MATRIX>	transforms;
	};

	// Animation clip with duration and list of keyframes
	struct SIMPLE_ANIM_CLIP
	{
		double								duration;
		std::vector<SIMPLE_ANIM_FRAME>		frames;
	};
#pragma endregion


#pragma region Functions
	// Returns count of polygons in scene, or error code if failed
	FBXLIB_INTERFACE int GetScenePolyCount(const char* _fbx_filepath);

	// Extracts mesh from scene and converts it for loading
	// Returns result code
	// NOTES:
	//   Use _meshName to search for mesh by name, otherwise first mesh is used by default
	//   Use _meshElements to specify which supported elements to store (others default to 0)
	FBXLIB_INTERFACE int ExtractMesh(const char* _fbx_filepath, SIMPLE_MESH& _mesh, const char* _meshName = nullptr, int32_t _elementOptions = 0);

	// Returns count of materials in scene, or error code if failed
	FBXLIB_INTERFACE int GetSceneMaterialCount(const char* _fbx_filepath);

	// Extracts material from scene and converts it for loading
	// Returns result code
	// NOTES:
	//   Use _matElements to specify which supported elements to store (others default to 0)
	FBXLIB_INTERFACE int ExtractMaterial(const char* _fbx_filepath, SIMPLE_MATERIAL_LIST& _material_list, uint32_t _matNum = 0, int32_t _elementOptions = 0);

	// Returns count of poses in scene, or error code if failed
	FBXLIB_INTERFACE int GetScenePoseCount(const char* _fbx_filepath);

	// Extracts animation from scene and converts it for loading
	// Returns result code
	// NOTES:
	//   Exports at 30 frames per second
	FBXLIB_INTERFACE int ExtractAnimation(const char* _fbx_filepath, SIMPLE_ANIM_CLIP& _anim_clip, uint32_t _elementOptions = 0);
#pragma endregion

}
