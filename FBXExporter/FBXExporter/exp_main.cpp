#include <iostream>

#include "exp_interface.h"


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
				<< FBXLibrary::MESH_ELEMENT::POSITION << " - Positions; "
				<< FBXLibrary::MESH_ELEMENT::NORMAL << " - Normals; "
				<< FBXLibrary::MESH_ELEMENT::COLOR << " - Colors; "
				<< FBXLibrary::MESH_ELEMENT::TEXCOORD << " - Texture coordinates; "
				<< FBXLibrary::MESH_ELEMENT::ALL << " - All"
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
				<< FBXLibrary::MATERIAL_ELEMENT::DIFFUSE << " - Diffuse; "
				<< FBXLibrary::MATERIAL_ELEMENT::EMISSIVE << " - Emissive; "
				<< FBXLibrary::MATERIAL_ELEMENT::SPECULAR << " - Specular; "
				<< FBXLibrary::MATERIAL_ELEMENT::NORMALMAP << " - Normal map; "
				<< FBXLibrary::MATERIAL_ELEMENT::ALL << " - All"
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
		int result = FAIL;

		// extract animation data
		result = FBXExporter::ExtractAnimationFromFbxFile(filepath, 0, exports[EXPORT_OPTION::INDEX_ANIMATION]);

		// extract material data
		result = FBXExporter::ExtractMaterialsFromFbxFile(filepath, elementOptions[EXPORT_OPTION::INDEX_MATERIAL], exports[EXPORT_OPTION::INDEX_MATERIAL]);

		// extract mesh data
		result = FBXExporter::ExtractMeshFromFbxFile(filepath, nullptr, elementOptions[EXPORT_OPTION::INDEX_MESH], exports[EXPORT_OPTION::INDEX_MESH]);
	}
#pragma endregion
}


int main(int argc, char* argv[])
{
	// set automatic memory leak reporting on program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// verify filename was specified
	if (argc > 1)
	{
		// store fbx filepath
		filepath = argv[1];

		// read export and element selections
		if (ReadOptions())
			// if valid selection was made, extract (and export to file if selected) data from .fbx file
			ExtractAndExportData();
	}
	// if no filename was specified, print error
	else
		std::cout << "No file to import" << std::endl;

	// print exit prompt
	std::cout << "Press enter to exit" << std::endl;
	std::cin.get();
}
