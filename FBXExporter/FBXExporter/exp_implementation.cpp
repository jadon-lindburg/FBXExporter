#include <cstring>
#include <vector>

#include "..\FBXLibrary\lib_debug.h"
#include "exp_interface.h"
#include "exp_utility.h"


namespace FBXExporter
{
#pragma region Variables
	FBXLibrary::SIMPLE_MESH mesh;
	FBXLibrary::SIMPLE_MATERIAL_LIST materials;
	FBXLibrary::SIMPLE_ANIM_CLIP animation;
#pragma endregion


#pragma region Utility Functions
	void ReplaceExtension(char* _outFile, const char* _inFile, const char* _extension)
	{
		int pathLen = (int)(strrchr(_inFile, '.') - _inFile);
		int extensionLen = (int)strlen(_extension);

		memset(_outFile, 0, pathLen + extensionLen + 1);
		memcpy(&_outFile[0], _inFile, pathLen);
		memcpy(&_outFile[pathLen], _extension, extensionLen);
	}

	int ExportMesh(FBXLibrary::SIMPLE_MESH _mesh)
	{
		// TODO:
// rework exporting

/*
// open or create output file for writing
std::fstream fout = std::fstream(_outputFilepath, std::ios_base::out | std::ios_base::binary);

// verify file is open
if (fout.is_open())
{
	uint32_t numVerts = (uint32_t)verts_compactified.size();
	uint32_t numInds = (uint32_t)indices.size();
	uint32_t numBytes = sizeof(numVerts) + sizeof(numInds) + (numVerts * sizeof(SIMPLE_VERTEX)) + (numInds * sizeof(uint32_t));
	float reductionAmount = (verts_raw.size() - verts_compactified.size()) / (verts_raw.size() * 1.0f) * 100;

	// write data to file with format:
	//   uint32_t											: number of vertices
	//   { float3, float3, float4, float2 }[numVerts]		: vertex data
	//   uint32_t											: number of indices
	//   uint32_t[numInds]									: index data
	fout.write((const char*)&numVerts, sizeof(numVerts));
	fout.write((const char*)&verts_compactified[0], numVerts * sizeof(SIMPLE_VERTEX));
	fout.write((const char*)&numInds, sizeof(numInds));
	fout.write((const char*)&indices[0], numInds * sizeof(uint32_t));


	std::cout
		<< "Raw vertex count : " << verts_raw.size() << std::endl
		<< "Unique vertex count : " << verts_compactified.size() << std::endl
		<< "Reduction : " << reductionAmount << "%" << std::endl
		<< "Index count : " << indices.size() << std::endl
		<< "Wrote " << numBytes << " bytes to file" << std::endl
		<< std::endl;


	result = 0;
}
*/

		return SUCCESS;
	}
	int ExportMaterial(FBXLibrary::SIMPLE_MATERIAL_LIST _materials)
	{
		return SUCCESS;
	}
	int ExportAnimation(FBXLibrary::SIMPLE_ANIM_CLIP animation)
	{
		return SUCCESS;
	}
#pragma endregion

#pragma region Interface Functions
	int ExtractMesh(const char* _inFile, const char* _meshName, uint32_t _meshElements, bool _export)
	{
		int result = FAIL;

		// get output filename
		char outFile[260];
		ReplaceExtension(outFile, _inFile, ".mesh");

		// extract mesh data from file
		result = FBXLibrary::ExtractMesh(_inFile, outFile, mesh, nullptr, _meshElements);
		if (FAILED(result))
			return result;

		// export data to file if flag is set
		if (_export)
			result = ExportMesh(mesh);

		return result;
	}
	int ExtractMaterials(const char* _inFile, uint32_t _matElements, bool _export)
	{
		int result = FAIL;

		// get output filename
		char outFile[260];
		ReplaceExtension(outFile, _inFile, ".mat");

		// extract material data from file
		result = FBXLibrary::ExtractMaterial(_inFile, outFile, materials, _matElements);
		if (FAILED(result))
			return result;

		// export data to file if flag is set
		if (_export)
			result = ExportMaterial(materials);

		return result;
	}
	int ExtractAnimation(const char* _inFile, uint32_t _animElements, bool _export)
	{
		int result = FAIL;

		// get output filename
		char outFile[260];
		ReplaceExtension(outFile, _inFile, ".anim");

		// extract animation data from file
		result = FBXLibrary::ExtractAnimation(_inFile, outFile, animation);
		if (FAILED(result))
			return result;

		// export data to file if flag is set
		if (_export)
			result = ExportAnimation(animation);

		return result;
	}
#pragma endregion

}
