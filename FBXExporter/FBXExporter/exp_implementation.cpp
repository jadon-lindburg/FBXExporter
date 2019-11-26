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

	int ExportMesh(const char* _outFile, FBXLibrary::SIMPLE_MESH _mesh)
	{
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
	int ExportMaterial(const char* _outFile, FBXLibrary::SIMPLE_MATERIAL_LIST _materials)
	{
		/*
		// open or create output file for writing
		std::fstream fout = std::fstream(_outputFilepath, std::ios_base::out | std::ios_base::binary);

		// verify file is open
		if (fout.is_open())
		{
			uint32_t numMats = (uint32_t)materials.size();
			uint32_t numPaths = (uint32_t)filepaths.size();
			uint32_t numBytes = sizeof(numMats) + sizeof(numPaths) + (numMats * sizeof(SIMPLE_MATERIAL)) + (numPaths * sizeof(filepath_t));

			// write data to file with format:
			//   uint32_t												: number of materials
			//   { float3, float, uint64_t }[compType::COUNT][numMats]	: material data
			//   uint32_t												: number of filepaths
			//   filepath_t[numPaths]									: filepath data
			fout.write((const char*)&numMats, sizeof(numMats));
			fout.write((const char*)&materials[0], numMats * sizeof(SIMPLE_MATERIAL));
			fout.write((const char*)&numPaths, sizeof(numPaths));
			fout.write((const char*)&filepaths[0], numPaths * sizeof(filepath_t));


			std::cout
				<< "Number of components exported : " << filepaths.size() << std::endl
				<< "Filepaths : " << std::endl;
			for (uint8_t i = 0; i < filepaths.size(); i++)
			{
				char path[260];
				memcpy(path, &filepaths[i][0], 260);
				std::cout << path << std::endl;
			}
			std::cout
				<< "Wrote " << numBytes << " bytes to file" << std::endl
				<< std::endl;


			result = 0;
		}
		*/

		return SUCCESS;
	}
	int ExportAnimation(const char* _outFile, FBXLibrary::SIMPLE_ANIM_CLIP animation)
	{
		/*
		// open or create output file for writing
		std::fstream fout = std::fstream(_outputFilepath, std::ios_base::out | std::ios_base::binary);

		// verify file is open
		if (fout.is_open())
		{
			uint32_t numJoints = (uint32_t)joints_out.size();
			uint32_t frameSize = sizeof(double) + (numJoints * sizeof(SIMPLE_MATRIX));
			uint32_t numFrames = (uint32_t)animClip.frames.size();
			uint32_t numBytes = sizeof(numJoints) + sizeof(numFrames) + (numJoints * sizeof(SIMPLE_JOINT)) + sizeof(animClip.duration) + (numFrames * frameSize);

			// write bind pose to file with format:
			//   uint32_t										: number of joints
			//   { float[16], int }[numJoints]					: joint data
			fout.write((char*)&numJoints, sizeof(numJoints));
			fout.write((char*)&joints_out[0], numJoints * sizeof(SIMPLE_JOINT));

			// write animation clip to file with format:
			//   double											: animation duration in seconds
			//   uint32_t										: byte length of each frame
			//   uint32_t										: number of frames
			//   { double, float[16][numJoints] }[numFrames]	: frame data
			fout.write((char*)&animClip.duration, sizeof(animClip.duration));
			fout.write((char*)&frameSize, sizeof(frameSize));
			fout.write((char*)&numFrames, sizeof(numFrames));
			for (uint32_t i = 0; i < numFrames; i++)
			{
				fout.write((char*)&animClip.frames[i].time, sizeof(SIMPLE_ANIM_FRAME::time));
				fout.write((char*)&animClip.frames[i].transforms[0], frameSize - sizeof(SIMPLE_ANIM_FRAME::time));
			}


			std::cout
				<< "Joint count : " << numJoints << std::endl
				<< "Duration : " << animClip.duration << std::endl
				<< "Frame byte length : " << frameSize << std::endl
				<< "Frame count : " << numFrames << std::endl
				<< "Wrote " << numBytes << " bytes to file" << std::endl
				<< std::endl;


			result = 0;
		}
		*/

		return SUCCESS;
	}
#pragma endregion

#pragma region Interface Functions
	int ExtractMeshFromFbxFile(const char* _inFile, const char* _meshName, uint32_t _elementOptions, bool _export)
	{
		int result = FAIL;

		// get output filename
		char outFile[260];
		ReplaceExtension(outFile, _inFile, ".mesh");

		// extract mesh data from file
		result = FBXLibrary::ExtractMesh(_inFile, mesh, nullptr, _elementOptions);
		if (FAILED(result))
			return result;

		// export data to file if flag is set
		if (_export)
			result = ExportMesh(outFile, mesh);

		return result;
	}
	int ExtractMaterialsFromFbxFile(const char* _inFile, uint32_t _elementOptions, bool _export)
	{
		int result = FAIL;

		// get output filename
		char outFile[260];
		ReplaceExtension(outFile, _inFile, ".mat");

		// extract material data from file
		result = FBXLibrary::ExtractMaterial(_inFile, materials, 0, _elementOptions);
		if (FAILED(result))
			return result;

		// export data to file if flag is set
		if (_export)
			result = ExportMaterial(outFile, materials);

		return result;
	}
	int ExtractAnimationFromFbxFile(const char* _inFile, uint32_t _elementOptions, bool _export)
	{
		int result = FAIL;

		// get output filename
		char outFile[260];
		ReplaceExtension(outFile, _inFile, ".anim");

		// extract animation data from file
		result = FBXLibrary::ExtractAnimation(_inFile, animation, _elementOptions);
		if (FAILED(result))
			return result;

		// export data to file if flag is set
		if (_export)
			result = ExportAnimation(outFile, animation);

		return result;
	}
#pragma endregion

}
