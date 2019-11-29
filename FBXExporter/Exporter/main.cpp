#include "interface.h"

#include <iostream>

#include "../Library/debug.h"

namespace
{
	// Data types available to export from a .fbx file.
	struct DataTypeOption
	{
		enum
		{
			MESH = 0x00000001  // Export .mesh file.
			, MATERIAL = 0x00000002  // Export .mat file.
			, ANIMATION = 0x00000004  // Export .anim file.
			, ALL = MESH | MATERIAL | ANIMATION  // Export all supported data types.
		};
	};

	char*								filepath = nullptr;
	char								buffer[50];
	uint32_t							exportSelections = 0;
	uint32_t							elementOptions[fbx_exporter::library::DataTypeIndex::COUNT] = {};
	fbx_exporter::FileReadMode			dataTypesToExport[fbx_exporter::library::DataTypeIndex::COUNT] = {};

	/* Reads and stores export options
	  RETURNS
		true : A valid export selection was made.
		false : No valid export selection was made.
	*/
	bool ReadOptions()
	{
		std::cout << "File to Export : ";
		if (filepath != nullptr)
			std::cout << filepath;
		std::cout << std::endl << std::endl;

		std::cout << "Export options : "
			<< DataTypeOption::MESH << " - Mesh; "
			<< DataTypeOption::MATERIAL << " - Materials; "
			<< DataTypeOption::ANIMATION << " - Animation; "
			<< DataTypeOption::ALL << " - All"
			<< std::endl
			<< "Enter sum of selections : ";

		// read export selections
		std::cin.getline(buffer, 50);
		exportSelections = strtol(buffer, nullptr, 10);
		std::cout << std::endl;

		// convert selections into array
		for (uint32_t i = 0; i < fbx_exporter::library::DataTypeIndex::COUNT; i++)
		{
			if (exportSelections & (uint32_t)pow(2, i))
				dataTypesToExport[i] = fbx_exporter::FileReadMode::EXPORT;
			else
				dataTypesToExport[i] = fbx_exporter::FileReadMode::EXTRACT;
		}

		// if no valid selection was made, print error and return false
		if (!(
			(dataTypesToExport[fbx_exporter::library::DataTypeIndex::MESH]
				== fbx_exporter::FileReadMode::EXPORT)
			|| (dataTypesToExport[fbx_exporter::library::DataTypeIndex::MATERIAL]
				== fbx_exporter::FileReadMode::EXPORT)
			|| (dataTypesToExport[fbx_exporter::library::DataTypeIndex::ANIMATION]
				== fbx_exporter::FileReadMode::EXPORT)
			))
		{
			std::cout << "Invalid selection" << std::endl;
			return false;
		}

		if (dataTypesToExport[fbx_exporter::library::DataTypeIndex::MESH]
			== fbx_exporter::FileReadMode::EXPORT)
		{
			std::cout << "Mesh elements supported : "
				<< static_cast<int>(fbx_exporter::library::MeshElement::POSITION)
				<< " - Positions; "
				<< static_cast<int>(fbx_exporter::library::MeshElement::NORMAL)
				<< " - Normals; "
				<< static_cast<int>(fbx_exporter::library::MeshElement::COLOR)
				<< " - Colors; "
				<< static_cast<int>(fbx_exporter::library::MeshElement::TEXCOORD)
				<< " - Texture coordinates; "
				<< static_cast<int>(fbx_exporter::library::MeshElement::ALL)
				<< " - All"
				<< std::endl
				<< "Enter sum of selections : ";

			// read mesh options
			std::cin.getline(buffer, 50);
			elementOptions[fbx_exporter::library::DataTypeIndex::MESH] = strtol(buffer, nullptr, 10);
			std::cout << std::endl;
		}
		if (dataTypesToExport[fbx_exporter::library::DataTypeIndex::MATERIAL]
			== fbx_exporter::FileReadMode::EXPORT)
		{
			std::cout << "Material elements supported : "
				<< static_cast<int>(fbx_exporter::library::MaterialElement::DIFFUSE)
				<< " - Diffuse; "
				<< static_cast<int>(fbx_exporter::library::MaterialElement::EMISSIVE)
				<< " - Emissive; "
				<< static_cast<int>(fbx_exporter::library::MaterialElement::SPECULAR)
				<< " - Specular; "
				<< static_cast<int>(fbx_exporter::library::MaterialElement::NORMALMAP)
				<< " - Normal map; "
				<< static_cast<int>(fbx_exporter::library::MaterialElement::ALL)
				<< " - All"
				<< std::endl
				<< "Enter sum of selections : ";

			// read material options
			std::cin.getline(buffer, 50);
			elementOptions[fbx_exporter::library::DataTypeIndex::MATERIAL]
				= strtol(buffer, nullptr, 10);
			std::cout << std::endl;
		}
		if (dataTypesToExport[fbx_exporter::library::DataTypeIndex::ANIMATION]
			== fbx_exporter::FileReadMode::EXPORT)
		{
			std::cout << "No animation options supported"
				<< std::endl;
		}

		// if valid selection was made, return true
		return true;
	}
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
			fbx_exporter::GetDataFromFbxFile(filepath, elementOptions, dataTypesToExport);
	}
	else
	{
		std::cout << "No file to import" << std::endl;
	}

	std::cout << "Press enter to exit" << std::endl;
	std::cin.get();
}
