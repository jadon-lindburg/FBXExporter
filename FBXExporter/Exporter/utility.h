#ifndef _FBXEXPORTER_EXPORTER_UTILITY_H_
#define _FBXEXPORTER_EXPORTER_UTILITY_H_

#include "defines.h"

#include "../Library/defines.h"

namespace fbx_exporter
{
	/* Replaces the extension of a filepath.
	PARAMETERS
	  _in_filepath : The filepath to alter.
	  _in_extension : The extension to alter the filepath with.
	  _out_filepath : The altered filepath.
	*/
	void ReplaceExtension(
		const char*						_in_filepath
		, const char*					_in_extension
		, char*							_out_filepath
	);

	/* Exports mesh data to a file.
	PARAMETERS
	  _in_filepath : The filepath to export data to.
	  _in_mesh : The data to export.
	RETURNS
	  INVALID_ARG : An invalid argument was passed.
	  FAIL : File could not be opened.
	  EXPORT : Data was successfully exported to file.
	*/
	library::Result ExportMesh(
		const char*						_in_filepath
		, const library::Mesh			_in_mesh
	);

	/* Exports material data to a file.
	PARAMETERS
	  _in_filepath : The filepath to export data to.
	  _in_materials : The data to export.
	RETURNS
	  INVALID_ARG : An invalid argument was passed.
	  FAIL : File could not be opened.
	  EXPORT : Data was successfully exported to file.
	*/
	library::Result ExportMaterials(
		const char*						_in_filepath
		, library::MaterialList			_in_materials
	);

	/* Exports animation data to a file.
	PARAMETERS
	  _in_filepath : The filepath to export data to.
	  _in_animationClip : The data to export.
	RETURNS
	  INVALID_ARG : An invalid argument was passed.
	  FAIL : File could not be opened.
	  EXPORT : Data was successfully exported to file.
	*/
	library::Result ExportAnimation(
		const char*						_in_filepath
		, library::AnimationClip		_in_animationClip
	);

}

#endif // _FBXEXPORTER_EXPORTER_UTILITY_H_