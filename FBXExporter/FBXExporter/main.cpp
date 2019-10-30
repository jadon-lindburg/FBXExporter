#include <iostream>

#include "interface.h"


void PrintResult(int _result, const char* _message)
{
	if (_result == -1)
		std::cout << "FAILED : " << _message << std::endl << std::endl;
	else
		std::cout << "SUCCESSFUL : " << _message << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
	int result = -1;

	if (argc > 1)
	{
		char buffer[50];
		int32_t options = 0;

#define MESHEXPORT '1'
#define MATEXPORT '2'
#define ALLEXPORT '3'

		std::cout << "File to import : " << argv[1] << std::endl << std::endl;

		std::cout << "Import options : "
			<< MESHEXPORT << " - Mesh; "
			<< MATEXPORT << " - Materials; "
			<< ALLEXPORT << " - All"
			<< std::endl;

		std::cout << "Enter selection : ";
		std::cin.getline(buffer, 50);
		char exports = buffer[0];

		std::cout << std::endl;

		switch (exports)
		{
		case ALLEXPORT:
		case MESHEXPORT:
#define POSITION 0x00000001
#define NORMAL 0x00000002
#define SPECULAR 0x00000004
#define TEXCOORD 0x00000008

			std::cout << "Mesh elements supported : "
				<< POSITION << " - Positions; "
				<< NORMAL << " - Normals; "
				<< SPECULAR << " - Colors; "
				<< TEXCOORD << " - Texture coordinates"
				<< std::endl;

			std::cout << "Enter sum of selections : ";
			std::cin.getline(buffer, 50);
			options = strtol(buffer, nullptr, 10);

			std::cout << std::endl;

			std::cout << "Exporting mesh..." << std::endl;
			result = FBXExporter::Mesh::ExportMesh(argv[1], nullptr, options);
			PrintResult(result, "Export mesh");

			if (exports != ALLEXPORT)
				break;

		case MATEXPORT:
#define DIFFUSE 0x00000001
#define EMISSIVE 0x00000002
#define SPECULAR 0x00000004
#define NORMALMAP 0x00000008

			std::cout << "Material elements supported : "
				<< DIFFUSE << " - Diffise; "
				<< EMISSIVE << " - Emissive; "
				<< SPECULAR << " - Specular; "
				<< NORMALMAP << " - Normal map"
				<< std::endl;

			std::cout << "Enter sum of selections : ";
			std::cin.getline(buffer, 50);
			options = strtol(buffer, nullptr, 10);

			std::cout << std::endl;

			std::cout << "Exporting materials..." << std::endl;
			result = FBXExporter::Material::ExportMaterials(argv[1], options);
			PrintResult(result, "Export materials");

			break;

		default:
			std::cout << "Invalid selection" << std::endl;
		}
	}
	else
		std::cout << "No file to import" << std::endl;

	std::cout << "Press enter to exit" << std::endl;
	std::cin.get();
}
