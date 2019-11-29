#ifndef _FBXEXPORTER_LIBRARY_DEFINES_H_
#define _FBXEXPORTER_LIBRARY_DEFINES_H_

#include <array>
#include <vector>

namespace fbx_exporter
{
	namespace library
	{
		// Char array of maximum allowed Windows filepath length.
		using filepath_t = std::array<char, 260>;


		// Named index values for array access.
		struct DataTypeIndex
		{
			enum
			{
				MESH = 0
				, MATERIAL
				, ANIMATION
				, COUNT
			};
		};

		// Indicates vertex elements to store when extracting a mesh.
		enum struct MeshElement
		{
			POSITION = 0x00000001  // Model-space position vector.
			, NORMAL = 0x00000002  // Model-space normal vector.
			, COLOR = 0x00000004  // RGBA color.
			, TEXCOORD = 0x00000008  // UV texcure coordinate.
			, ALL = POSITION | NORMAL | COLOR | TEXCOORD  // All supported elements.
		};

		// Indicates textures to store when extracting a material.
		enum struct MaterialElement
		{
			DIFFUSE = 0x00000001  // Diffuse texture.
			, EMISSIVE = 0x00000002  // Emissive texture.
			, SPECULAR = 0x00000004  // Specular map.
			, NORMALMAP = 0x00000008  // Normal map.
			, ALL = EMISSIVE | DIFFUSE | SPECULAR | NORMALMAP  // All supported elements.
		};

		// Indicates animation elements to store when extracting an animation.
		enum struct AnimationElement
		{
		};

		// Indicates the result of a function or operation.
		enum struct Result
		{
			INVALID_ARG = -2  // An invalid argument was passed.
			, FAIL = -1  // The operation failed to complete.
			, SUCCESS = 0  // The operation completely successfully.
			, EXTRACT = 1  // Data was successfully extracted from a file.
			, EXPORT = 2  // Data was successfully exported to a file.
		};


		// 4x4 matrix data container.
		struct Matrix
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

		// Vertex data container.
		struct Vertex
		{
			float pos[3] = { 0.0f, 0.0f, 0.0f };  // Model-space position vector.
			float norm[3] = { 0.0f, 0.0f, 0.0f };  // Model-space normal vector.
			float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };  // RGBA color.
			float texCoord[2] = { 0.0f, 0.0f };  // UV texture coordinate.

			bool operator==(const Vertex rhs)
			{
				return
				{
					(pos[0] == rhs.pos[0] && pos[1] == rhs.pos[1] && pos[2] == rhs.pos[2])
					&& (norm[0] == rhs.norm[0] && norm[1] == rhs.norm[1] && norm[2] == rhs.norm[2])
					&& (color[0] == rhs.color[0] && color[1] == rhs.color[1]
					&& color[2] == rhs.color[2] && color[3] == rhs.color[3])
					&& (texCoord[0] == rhs.texCoord[0] && texCoord[1] == rhs.texCoord[1])
				};
			}
		};

		// Mesh data container.
		struct Mesh
		{
			uint32_t					vertex_count = 0;  // Number of vertices in mesh.
			uint32_t					index_count = 0;  // Number of indices in mesh.
			std::vector<Vertex>			vertices;  // List of vertices in mesh.
			std::vector<uint32_t>		indices;  // List of indices in mesh.
		};

		// Material data container.
		struct Material
		{
			enum ComponentType
			{
				DIFFUSE = 0
				, EMISSIVE
				, SPECULAR
				, NORMALMAP
				, COUNT
			};

			struct Component
			{
				float	value[3] = { 0.0f, 0.0f, 0.0f };  // Color value.
				float	factor = 0.0f;  // Texture strength factor.
				int64_t	input = -1;  // Index of texture filepath in list.
			};

			Component& operator[](int i) { return components[i]; }
			const Component& operator[](int i) const { return components[i]; }

		private:
			Component components[ComponentType::COUNT];
		};

		// Paired material list and filepath list.
		struct MaterialList
		{
			std::vector<Material>		materials;
			std::vector<filepath_t>		filepaths;
		};

		// Animation joint data container.
		struct AnimationJoint
		{
			Matrix	global_transform;  // Model-space joint transformation matrix.
			int		parent_index = -1;  // Index of parent joint. -1 indicates no parent.
		};

		// Animation frame data container.
		struct AnimationFrame
		{
			double						time;  // Trigger time for frame.
			std::vector<Matrix>			transforms;  // List of joint transformations.
		};

		// Animation clip data container.
		struct AnimationClip
		{
			double						duration;  // Animation length in seconds.
			std::vector<AnimationFrame>	frames;  // List of keyframes.
		};

	}
}

#endif // _FBXEXPORTER_LIBRARY_DEFINES_H_