#ifndef _FBXEXPORTER_FBXLIBRARY_UTILITY_H_
#define _FBXEXPORTER_FBXLIBRARY_UTILITY_H_

#include <cstdint>

#include "fbxsdk.h"
#pragma comment(lib, "libfbxsdk.lib")

#include "defines.h"

namespace fbx_exporter
{
	namespace library
	{
		// Stores animation joint data in intermediate form between FbxScene and AnimationJoint.
		struct AnimationJointFbx
		{
			FbxNode*	fbx_node_p;  // Pointer to the FbxNode for this joint.
			int			parent_index = -1;  // Index of joint's parent. -1 indicates no parent.
		};


		/* Creates an FBX sdk manager and imports data from a .fbx file into an FBX scene.
		  PARAMETERS
			_in_fbxFilepath : The path to the .fbx file to import data from.
			_out_fbxManager_p : Pointer to the FbxManager created.
			_out_fbxScene_p : Pointer to the FbxScene created and imported to.
		  RETURNS
			INVALID_ARG : An invalid argument was passed.
			FAIL : Manager was not created, scene was not created, or scene was not imported.
			SUCCESS : Manager and scene were created and scene was imported.
		*/
		Result CreateFbxManagerAndImportFbxScene(
			const char* _in_fbxFilepath, FbxManager*& _out_fbxManager_p,
			FbxScene*& _out_fbxScene_p);

		/* Converts matrix data from FbxAMatrix to Matrix form.
		  PARAMETERS
			_in_fbxMatrix : The matrix to convert.
		  RETURNS
			Matrix : The converted matrix.
		*/
		Matrix ConvertFbxAMatrixToMatrix(FbxAMatrix _in_fbxMatrix);

		/* Extracts mesh data from an FbxScene and stores it in a Mesh.
		  PARAMETERS
		    _in_fbxScene_p : The FBX scene to extract data from.
			_in_meshName : The mesh name to search the scene for, if desired.
			    Pass "" to get the first mesh from the file.
			_in_elementsToExtract : A bit - flag set denoting which vertex elements to store.
			_out_mesh : The mesh container to store extracted data in.
		  RETURNS
		    INVALID_ARG : An invalid argument was passed.
			EXTRACT : Data was extracted successfully.
		*/
		Result GetMeshFromFbxScene(
			const FbxScene* _in_fbxScene_p, const char* _in_meshName,
			int32_t _in_elementsToExtract, Mesh& _out_mesh);

		/* Extracts material data from an FbxScene and stores it in a Material.
		  PARAMETERS
			_in_fbxScene_p : The FBX scene to extract data from.
			_in_materialNum : The material number to get from the scene.
			_in_elementsToExtract : A bit-flag set denoting which texture elements to store.
			_out_materialList : The material and filepath container to store extracted data in.
		  RETURNS
			INVALID_ARG : An invalid argument was passed.
			EXTRACT : Data was successfully extracted.
		*/
		Result GetMaterialsFromFbxScene(
			const FbxScene* _in_fbxScene_p, uint32_t _in_materialNum,
			int32_t _in_elementsToExtract, MaterialList& _out_materialList);

		/* Extracts animation data from an FbxScene and stores it in an AnimationClip.
		  PARAMETERS
			_in_fbxScene_p : The FBX scene to extract data from.
			_in_elementsToExtract : A bit-flag set denoting which animation elements to store.
			_out_animationClip : The animation container to store extracted data in.
		  RETURNS
			INVALID_ARG : An invalid argument was passed.
			EXTRACT : Data was successfully extracted.
		  NOTES
			Extracts animations at 30 frames per second.
		*/
		Result GetAnimationFromFbxScene(
			const FbxScene* _in_fbxScene_p, uint32_t _in_elementsToExtract,
			AnimationClip& _out_animationClip);

	}
}

#endif // _FBXEXPORTER_FBXLIBRARY_LIB_UTILITY_H_