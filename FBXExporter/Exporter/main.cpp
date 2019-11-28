#include "interface.h"

#include <iostream>

#include "../Library/debug.h"

namespace
{
#pragma region Structs
	// data types available to exportfrom a .fbx file
	struct EXPORT_OPTION
	{
		enum {
			OPTION_MESH = 0x00000001
			, OPTION_MATERIAL = 0x00000002
			, OPTION_ANIMATION = 0x00000004
			, OPTION_ALL = OPTION_MESH | OPTION_MATERIAL | OPTION_ANIMATION

			, INDEX_MESH = 0
			, INDEX_MATERIAL
			, INDEX_ANIMATION
			, INDEX_COUNT
		};
	};
#pragma endregion

#pragma region Variables
	// fbx file to extract data from
	char* filepath = nullptr;

	// buffer to read input into
	char buffer[50];

	// data types to export
	int32_t exportSelections = 0;

	// element option selection
	int32_t elementOptions[EXPORT_OPTION::INDEX_COUNT] = {};

	// bool array filled from export selections
	bool exports[EXPORT_OPTION::INDEX_COUNT];
#pragma endregion

#pragma region Private Helper Functions
	// Reads and stores options
	// Returns true if a valid export selection was made, false otherwise
	bool ReadOptions()
	{
		// print filename
		std::cout << "File to Export : ";
		if (filepath != nullptr)
			std::cout << filepath;
		std::cout << std::endl << std::endl;

		// print export options prompt
		std::cout << "Export options : "
			<< EXPORT_OPTION::OPTION_MESH << " - Mesh; "
			<< EXPORT_OPTION::OPTION_MATERIAL << " - Materials; "
			<< EXPORT_OPTION::OPTION_ANIMATION << " - Animation; "
			<< EXPORT_OPTION::OPTION_ALL << " - All"
			<< std::endl
			<< "Enter sum of selections : ";

		// read export selections
		std::cin.getline(buffer, 50);
		exportSelections = strtol(buffer, nullptr, 10);
		std::cout << std::endl;

		// convert selections into bool array
		for (uint32_t i = 0; i < EXPORT_OPTION::INDEX_COUNT; i++)
			exports[i] = exportSelections & (uint32_t)pow(2, i);

		// if no valid selection was made, print error and return false
		if (!(
			exports[EXPORT_OPTION::INDEX_MESH]
			|| exports[EXPORT_OPTION::INDEX_MATERIAL]
			|| exports[EXPORT_OPTION::INDEX_ANIMATION]
			))
		{
			std::cout << "Invalid selection" << std::endl;
			return false;
		}

		// read and store element selections
		if (exports[EXPORT_OPTION::INDEX_MESH])
		{
			// print mesh option prompt
			std::cout << "Mesh elements supported : "
				<< fbx_exporter::library::MeshElement::POSITION << " - Positions; "
				<< fbx_exporter::library::MeshElement::NORMAL << " - Normals; "
				<< fbx_exporter::library::MeshElement::COLOR << " - Colors; "
				<< fbx_exporter::library::MeshElement::TEXCOORD << " - Texture coordinates; "
				<< fbx_exporter::library::MeshElement::ALL << " - All"
				<< std::endl
				<< "Enter sum of selections : ";

			// read mesh options
			std::cin.getline(buffer, 50);
			elementOptions[EXPORT_OPTION::INDEX_MESH] = strtol(buffer, nullptr, 10);
			std::cout << std::endl;
		}
		if (exports[EXPORT_OPTION::INDEX_MATERIAL])
		{
			// print material options prompt
			std::cout << "Material elements supported : "
				<< fbx_exporter::library::MaterialElement::DIFFUSE << " - Diffuse; "
				<< fbx_exporter::library::MaterialElement::EMISSIVE << " - Emissive; "
				<< fbx_exporter::library::MaterialElement::SPECULAR << " - Specular; "
				<< fbx_exporter::library::MaterialElement::NORMALMAP << " - Normal map; "
				<< fbx_exporter::library::MaterialElement::ALL << " - All"
				<< std::endl
				<< "Enter sum of selections : ";

			// read material options
			std::cin.getline(buffer, 50);
			elementOptions[EXPORT_OPTION::INDEX_MATERIAL] = strtol(buffer, nullptr, 10);
			std::cout << std::endl;
		}
		if (exports[EXPORT_OPTION::INDEX_ANIMATION])
		{
			std::cout << "No animation options supported"
				<< std::endl;
		}

		// if valid selection was made, return true
		return true;
	}

	// Extracts (and exports if selected) data from .fbx file
	void ExtractAndExportData()
	{
		// set default result
		fbx_exporter::library::Result result = fbx_exporter::library::Result::FAIL;

		// extract animation data
		result = fbx_exporter::ExtractAnimationFromFbxFile(filepath, 0, exports[EXPORT_OPTION::INDEX_ANIMATION]);

		// extract material data
		result = fbx_exporter::ExtractMaterialsFromFbxFile(filepath, elementOptions[EXPORT_OPTION::INDEX_MATERIAL], exports[EXPORT_OPTION::INDEX_MATERIAL]);

		// extract mesh data
		result = fbx_exporter::ExtractMeshFromFbxFile(filepath, nullptr, elementOptions[EXPORT_OPTION::INDEX_MESH], exports[EXPORT_OPTION::INDEX_MESH]);
	}
#pragma endregion
}


int main(int argc, char* argv[])
{
	// set automatic memory leak reporting on program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// read and act on input if filename was specified
	if (argc > 1)
	{
		// store fbx filepath
		filepath = argv[1];

		// read export and element selections
		if (ReadOptions())
			// if valid selection was made, extract and export data from .fbx file
			ExtractAndExportData();
	}
	else
	{
		std::cout << "No file to import" << std::endl;
	}

	std::cout << "Press enter to exit" << std::endl;
	std::cin.get();
}
