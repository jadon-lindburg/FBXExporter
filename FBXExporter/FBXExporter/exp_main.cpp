#include <iostream>

#include "exp_interface.h"


void PrintResult(int _result, const char* _message)
{
	if (_result < 0)
		std::cout << "FAILED : " << _message << std::endl << std::endl;
	else
		std::cout << "SUCCEEDED : " << _message << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
	// set automatic memory leak reporting on program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	int result = -1;

	if (argc > 1)
	{
		char* filepath = argv[1];

		char buffer[50];
		int32_t options = 0;

		enum ExportOptions
		{
			Mesh = 0x00000001
			, Material = 0x00000002
			, Animation = 0x00000004
			, All = Mesh | Material | Animation
		};

		std::cout << "File to Export : " << filepath << std::endl << std::endl;

		std::cout << "Export options : "
			<< ExportOptions::Mesh << " - Mesh; "
			<< ExportOptions::Material << " - Materials; "
			<< ExportOptions::Animation << " - Animation; "
			<< std::endl;

		std::cout << "Enter sum of selections : ";
		std::cin.getline(buffer, 50);
		int32_t exports = strtol(buffer, nullptr, 10);

		std::cout << std::endl;

		if (exports & ExportOptions::Mesh)
		{
#define POSITION 0x00000001
#define NORMAL 0x00000002
#define COLOR 0x00000004
#define TEXCOORD 0x00000008

			std::cout << "Mesh elements supported : "
				<< POSITION << " - Positions; "
				<< NORMAL << " - Normals; "
				<< COLOR << " - Colors; "
				<< TEXCOORD << " - Texture coordinates"
				<< std::endl;

			std::cout << "Enter sum of selections : ";
			std::cin.getline(buffer, 50);
			options = strtol(buffer, nullptr, 10);

			std::cout << std::endl;

			std::cout << "Exporting mesh..." << std::endl;
			result = FBXExporter::ExtractMesh(filepath, nullptr, options);
			PrintResult(result, "Export mesh");

#undef POSITION
#undef NORMAL
#undef COLOR
#undef TEXCOORD			
		}

		if (exports & ExportOptions::Material)
		{
#define DIFFUSE 0x00000001
#define EMISSIVE 0x00000002
#define SPECULAR 0x00000004
#define NORMALMAP 0x00000008

			std::cout << "Material elements supported : "
				<< DIFFUSE << " - Diffuse; "
				<< EMISSIVE << " - Emissive; "
				<< SPECULAR << " - Specular; "
				<< NORMALMAP << " - Normal map"
				<< std::endl;

			std::cout << "Enter sum of selections : ";
			std::cin.getline(buffer, 50);
			options = strtol(buffer, nullptr, 10);

			std::cout << std::endl;

			std::cout << "Exporting materials..." << std::endl;
			result = FBXExporter::ExtractMaterials(filepath, options);
			PrintResult(result, "Export materials");

#undef DIFFUSE
#undef EMISSIVE
#undef SPECULAR
#undef NORMALMAP
		}

		if (exports & ExportOptions::Animation)
		{
			result = FBXExporter::ExtractAnimation(filepath);
			PrintResult(result, "Export animation");
		}

		if (!(exports & ExportOptions::Mesh) && !(exports & ExportOptions::Material) && !(exports & ExportOptions::Animation))
			std::cout << "Invalid selection" << std::endl;
	}

	else
		std::cout << "No file to import" << std::endl;

	std::cout << "Press enter to exit" << std::endl;
	std::cin.get();
}
