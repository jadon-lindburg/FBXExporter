#ifndef _FBXEXPORTER_EXPORTER_UTILITY_H_
#define _FBXEXPORTER_EXPORTER_UTILITY_H_


namespace fbx_exporter
{
	void ReplaceExtension(const char* _in_filepath, const char* _in_extension, char* _out_filepath);

	int ExportMesh(fbx_exporter::library::Mesh _in_mesh);

	int ExportMaterial(fbx_exporter::library::Material _in_material);

	int ExportAnimation(fbx_exporter::library::AnimationClip _in_animationClip);

}

#endif // _FBXEXPORTER_EXPORTER_UTILITY_H_