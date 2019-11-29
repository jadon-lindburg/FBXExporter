#ifndef _FBXEXPORTER_EXPORTER_INTERFACE_H_
#define _FBXEXPORTER_EXPORTER_INTERFACE_H_

#include "defines.h"

#include "../Library/defines.h"
#include "../Library/interface.h"

namespace fbx_exporter
{
	/* Extracts, stores, and optionally exports mesh data from a .fbx file.
	  PARAMETERS
	    _in_fbxFilepath : The path to the .fbx file to read from.
		_in_elementsToExtract : A bit-flag set indicating which vertex elements to store.
		_in_readMode : A value indicating how to use the data from the file.
		  DEFAULT : FileReadMode::EXTRACT
	  RETURNS
	    INVALID_ARG : An invalid argument was passed.
		FAIL : File could not be opened.
		EXTRACT : Data was extracted successfully.
	*/
	library::Result GetMeshFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t				_in_elementsToExtract
		, const FileReadMode			_in_readMode = FileReadMode::EXTRACT
	);

	/* Extracts, stores, and optionally exports mesh data from a .fbx file.
	  PARAMETERS
		_in_fbxFilepath : The path to the .fbx file to read from.
		_in_elementsToExtract : A bit-flag set indicating which texture elements to store.
		_in_readMode : A value indicating how to use the data from the file.
		  DEFAULT : FileReadMode::EXTRACT
	  RETURNS
		INVALID_ARG : An invalid argument was passed.
		FAIL : File could not be opened.
		EXTRACT : Data was extracted successfully.
	*/
	library::Result GetMaterialsFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t				_in_elementsToExtract
		, const FileReadMode			_in_readMode = FileReadMode::EXTRACT
	);

	/* Extracts, stores, and optionally exports mesh data from a .fbx file.
	  PARAMETERS
		_in_fbxFilepath : The path to the .fbx file to read from.
		_in_elementsToExtract : A bit-flag set indicating which animation elements to store.
		_in_readMode : A value indicating how to use the data from the file.
		  DEFAULT : FileReadMode::EXTRACT
	  RETURNS
		INVALID_ARG : An invalid argument was passed.
		FAIL : File could not be opened.
		EXTRACT : Data was extracted successfully.
	*/
	library::Result GetAnimationFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t				_in_elementsToExtract
		, const FileReadMode			_in_readMode = FileReadMode::EXTRACT
	);

	/* Extracts, stores, and optionally exports mesh data from a .fbx file.
	  PARAMETERS
		_in_fbxFilepath : The path to the .fbx file to read from.
		_in_elementsToExtract : A bit-flag set array indicating which data elements to store.
		_in_readMode : A value array indicating how to use the data from the file.
		  DEFAULT : FileReadMode::EXTRACT
	  RETURNS
		INVALID_ARG : An invalid argument was passed.
		FAIL : File could not be opened.
		EXTRACT : Data was extracted successfully.
	*/
	library::Result GetDataFromFbxFile(
		const char*						_in_fbxFilepath
		, const uint32_t*				_in_elementsToExtract
		, const FileReadMode*			_in_readModes
	);

}

#endif // _FBXEXPORTER_EXPORTER_INTERFACE_H_