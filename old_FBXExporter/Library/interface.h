#ifndef _FBXEXPORTER_LIBRARY_INTERFACE_H_
#define _FBXEXPORTER_LIBRARY_INTERFACE_H_

#include <cstdint>

#include "defines.h"

#ifdef FBXLIB_EXPORTS
#define FBXLIB_INTERFACE __declspec(dllexport)
#else
#define FBXLIB_INTERFACE __declspec(dllimport)
#endif

namespace fbx_exporter
{
	namespace library
	{
		/* Converts a Result into a boolean value.
		  PARAMETERS
			_in_r : The Result to check.
		  RETURNS
			true : The Result is a succeeding value
			false : The Result is a failing value
		*/
		FBXLIB_INTERFACE bool inline Succeeded(Result _in_r);

		/* Extracts mesh data from a .fbx file and stores it in a Mesh.
		  PARAMETERS
			_in_fbxFilepath : The path to the .fbx file to read from.
			_in_meshName : The mesh name to search the .fbx file for, if desired.
				Pass "" to get the first mesh from the file.
			_in_elementsToExtract : A bit-flag set indicating which vertex elements to store.
			_out_mesh : The mesh container to store extracted data in.
		  RETURNS
			INVALID_ARG : An invalid argument was passed.
			EXTRACT : Data was successfully extracted.
		*/
		FBXLIB_INTERFACE Result GetMeshFromFbxFile(
			const char*					_in_fbxFilepath
			, const char*				_in_meshName
			, const uint32_t			_in_elementsToExtract
			, Mesh&						_out_mesh
		);

		/* Extracts material data from a .fbx file and stores it in a Material.
		  PARAMETERS
			_in_fbxFilepath : The path to the .fbx file to read from.
			_in_materialNum : The material number to get from the .fbx file.
			_in_elementsToExtract : A bit-flag set indicating which texture elements to store.
			_out_materialList : The material and filepath container to store extracted data in.
		  RETURNS
			INVALID_ARG : An invalid argument was passed.
			EXTRACT : Data was successfully extracted.
		*/
		FBXLIB_INTERFACE Result GetMaterialsFromFbxFile(
			const char*					_in_fbxFilepath
			, const uint32_t			_in_materialNum
			, const uint32_t			_in_elementsToExtract
			, MaterialList&				_out_materialList
		);

		/* Extracts animation data from a .fbx file and stores it in an AnimationClip.
		  PARAMETERS
			_in_fbxFilepath : The path to the .fbx file to read from.
			_in_elementsToExtract : A bit-flag set indicating which animation elements to store.
			_out_animationClip : The animation container to store extracted data in.
		  RETURNS
			INVALID_ARG : An invalid argument was passed.
			EXTRACT : Data was successfully extracted.
		  NOTES
			Extracts animations at 30 frames per second.
		*/
		FBXLIB_INTERFACE Result GetAnimationFromFbxFile(
			const char*					_in_fbxFilepath
			, const uint32_t			_in_elementsToExtract
			, AnimationClip&			_out_animationClip
		);

	}
}

#endif // _FBXEXPORTER_LIBRARY_INTERFACE_H_