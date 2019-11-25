#pragma once


namespace FBXExporter
{
	void ReplaceExtension(char* _outFile, const char* _inFile, const char* _extension);

	int ExportMesh(FBXLibrary::SIMPLE_MESH _mesh);

	int ExportMaterial();

	int ExportAnimation();

}