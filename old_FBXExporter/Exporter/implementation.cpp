#include "interface.h"
#include "utility.h"



#include "../Library/debug.h"


namespace fbx_exporter
{
#pragma region Variables
	library::Mesh mesh;
	library::MaterialList materials;
	library::AnimationClip animation;
#pragma endregion

#pragma region Utility Function Definitions
	void ReplaceExtension(
		const char*						_in_filepath
		, const char*					_in_extension
		, char*							_out_filepath
	) {
		int pathLen = (int)(strrchr(_in_filepath, '.') - _in_filepath);
		int extensionLen = (int)strlen(_in_extension);

		memset(_out_filepath, 0, pathLen + extensionLen + 1);
		memcpy(&_out_filepath[0], _in_filepath, pathLen);
		memcpy(&_out_filepath[pathLen], _in_extension, extensionLen);
	}

	library::Result ExportMesh(
		const char*						_in_filepath
		, const library::Mesh			_in_mesh
	) {
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

		return library::Result::SUCCESS;
	}
	library::Result ExportMaterials(
		const char*						_in_filepath
		, library::MaterialList			_in_materials
	) {
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

		return library::Result::SUCCESS;
	}
	library::Result ExportAnimation(
		const char*						_in_filepath
		, library::AnimationClip		_in_animationClip
	) {
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

		return library::Result::SUCCESS;
	}
#pragma endregion

#pragma region Interface Functions
	library::Result GetMeshFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t				_in_elementsToExtract
		, const FileReadMode			_in_readMode
	) {
		library::Result ret_result = library::Result::FAIL;

		char exportFilepath[260];
		ReplaceExtension(_in_fbxFilepath, ".mesh", exportFilepath);

		ret_result = library::GetMeshFromFbxFile(_in_fbxFilepath, "", _in_elementsToExtract, mesh);
		if (!library::Succeeded(ret_result))
			return ret_result;

		if (_in_readMode == FileReadMode::EXPORT)
			ret_result = ExportMesh(exportFilepath, mesh);
		return ret_result;
	}
	library::Result GetMaterialsFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t				_in_elementsToExtract
		, const FileReadMode			_in_readMode
	) {
		library::Result ret_result = library::Result::FAIL;

		char exportFilepath[260];
		ReplaceExtension(_in_fbxFilepath, ".mat", exportFilepath);

		ret_result = library::GetMaterialsFromFbxFile(_in_fbxFilepath, 0, _in_elementsToExtract, materials);
		if (!library::Succeeded(ret_result))
			return ret_result;

		if (_in_readMode == FileReadMode::EXPORT)
			ret_result = ExportMaterials(exportFilepath, materials);
		return ret_result;
	}
	library::Result GetAnimationFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t				_in_elementsToExtract
		, const FileReadMode			_in_readMode
	) {
		library::Result ret_result = library::Result::FAIL;

		char exportFilepath[260];
		ReplaceExtension(_in_fbxFilepath, ".anim", exportFilepath);

		ret_result = library::GetAnimationFromFbxFile(_in_fbxFilepath, _in_elementsToExtract, animation);
		if (!library::Succeeded(ret_result))
			return ret_result;

		if (_in_readMode == FileReadMode::EXPORT)
			ret_result = ExportAnimation(exportFilepath, animation);
		return ret_result;
	}
	library::Result GetDataFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t*				_in_elementsToExtract
		, const FileReadMode*			_in_readModes
	) {
		library::Result ret_result = library::Result::FAIL;

		// animation must be extracted before mesh to include animation joint weights in mesh data
		ret_result = GetAnimationFromFbxFile(_in_fbxFilepath,
			_in_elementsToExtract[library::DataTypeIndex::ANIMATION],
			_in_readModes[library::DataTypeIndex::ANIMATION]);
		if (!library::Succeeded(ret_result))
			return ret_result;

		ret_result = GetMeshFromFbxFile(_in_fbxFilepath,
			_in_elementsToExtract[library::DataTypeIndex::MESH],
			_in_readModes[library::DataTypeIndex::MESH]);
		if (!library::Succeeded(ret_result))
			return ret_result;

		ret_result = GetMaterialsFromFbxFile(_in_fbxFilepath,
			_in_elementsToExtract[library::DataTypeIndex::MATERIAL],
			_in_readModes[library::DataTypeIndex::MATERIAL]);
		return ret_result;
	}
#pragma endregion

}
