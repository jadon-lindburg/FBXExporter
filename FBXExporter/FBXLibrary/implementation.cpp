#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include "interface.h"
#include "utility.h"

namespace FBXLibrary
{
	using filepath_t = std::array<char, 260>;

	// interface
	int GetScenePolyCount(const char* _fbxFilepath)
	{
		int result = -1;

		// scene pointer to be set by call to CreateAndImport()
		FbxScene* scene = nullptr;

		// create FbxManager and import scene from file
		FbxManager* sdkManager = CreateAndImport(_fbxFilepath, scene);

		// verify manager creation
		if (sdkManager == nullptr)
			return result;

		// verify scene import
		if (scene != nullptr)
		{
			result = 0;

			int geoCount = scene->GetGeometryCount();

			for (int i = 0; i < geoCount; i++)
			{
				FbxGeometry* geo = scene->GetGeometry(i);

				// skip non-mesh geometries
				if (geo->GetAttributeType() != FbxNodeAttribute::eMesh)
					continue;

				// add mesh's polygon count to result
				FbxMesh* mesh = (FbxMesh*)geo;
				result += mesh->GetPolygonCount();
			}
		}

		sdkManager->Destroy();

		return result;
	}

	int ExportSimpleMesh(const char* _fbxFilepath, const char* _outputFilepath, const char* _meshName, int32_t _meshElements)
	{
		int result = -1;

		// scene pointer to be set by call to CreateAndImport()
		FbxScene* scene = nullptr;

		// create FbxManager and import scene from file
		FbxManager* sdkManager = CreateAndImport(_fbxFilepath, scene);

		// verify manager creation
		if (sdkManager == nullptr)
			return result;

		// verify scene import
		if (scene != nullptr)
			result = ExtractMesh(scene, _outputFilepath, _meshName, _meshElements);

		sdkManager->Destroy();

		return result;
	}

	int GetSceneMaterialCount(const char* _fbxFilepath)
	{
		int result = -1;

		// scene pointer to be set by call to CreateAndImport()
		FbxScene* scene = nullptr;

		// create FbxManager and import scene from file
		FbxManager* sdkManager = CreateAndImport(_fbxFilepath, scene);

		// verify manager creation
		if (sdkManager == nullptr)
			return result;

		// verify scene import
		if (scene != nullptr)
			result = scene->GetMaterialCount();

		sdkManager->Destroy();

		return result;
	}

	int ExportSimpleMaterial(const char* _fbxFilepath, const char* _outputFilepath, uint32_t _matNum, int32_t _matElements)
	{
		int result = -1;

		// scene pointer to be set by call to CreateAndImport()
		FbxScene* scene = nullptr;

		// create FbxManager and import scene from file
		FbxManager* sdkManager = CreateAndImport(_fbxFilepath, scene);

		// verify manager creation
		if (sdkManager == nullptr)
			return result;

		// verify scene import
		if (scene != nullptr)
			result = ExtractMaterial(scene, _outputFilepath, _matNum, _matElements);

		sdkManager->Destroy();

		return result;
	}

	// utility
	FbxManager* CreateAndImport(const char* _fbxFilepath, FbxScene*& _scene)
	{
		// sdk manager handles memory management
		FbxManager* sdkManager = FbxManager::Create();

		// create IO settings object
		FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, "");
		sdkManager->SetIOSettings(ios);

		// create importer using sdk manager
		FbxImporter* importer = FbxImporter::Create(sdkManager, "");

		if (!importer->Initialize(_fbxFilepath, -1, sdkManager->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
			return nullptr;
		}

		// create new scene to populate with imported file
		_scene = FbxScene::Create(sdkManager, "imported_scene");

		// import contents of file into scene
		importer->Import(_scene);

		importer->Destroy();

		return sdkManager;
	}

	int ExtractMesh(const FbxScene* _scene, const char* _outputFilepath, const char* _meshName, int32_t _meshElements)
	{
		int result = -1;

		FbxScene* scene = (FbxScene*)_scene;
		FbxMesh* mesh = nullptr;

		// search scene for mesh name
		if (_meshName != nullptr)
		{
			// search scene for mesh name
			for (int i = 0; i < scene->GetGeometryCount(); i++)
			{
				FbxGeometry* geo = scene->GetGeometry(i);

				// if geometry is a mesh and name matches, store mesh
				if (geo->GetAttributeType() == FbxNodeAttribute::eMesh && geo->GetName() == _meshName)
				{
					mesh = (FbxMesh*)geo;
					break;
				}
			}
		}
		// if no mesh name is specified, use first mesh
		else
		{
			// find first mesh in scene
			for (int i = 0; i < scene->GetGeometryCount(); i++)
			{
				FbxGeometry* geo = scene->GetGeometry(i);

				if (geo->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					mesh = (FbxMesh*)geo;
					break;
				}
			}
		}

		if (mesh == nullptr)
			return result;

		// -- extract vertices from mesh --
		std::vector<simple_vertex_s> rawVerts;

		int polygonCount = mesh->GetPolygonCount();

		// get polygon vertices (equivalent to vertex indices)
		int* polygonVerts = mesh->GetPolygonVertices();
		// get control points (equivalent to vertices)
		const FbxVector4* controlPoints = mesh->GetControlPoints();

		// for each polygon in mesh
		for (int i = 0; i < polygonCount; i++)
		{
			// for each vertex in polygon
			for (int v = 0; v < 3; v++)
			{
				simple_vertex_s vert;

				int vertIndex = i * 3 + v;
				int pointIndex = polygonVerts[vertIndex];

				// get vertex position
				if (_meshElements & MeshElement::MeshPos)
				{
					FbxVector4 pos = controlPoints[pointIndex];

					vert.pos[0] = (float)pos[0];
					vert.pos[1] = (float)pos[1];
					vert.pos[2] = (float)pos[2];
				}

				// get vertex normal
				if (_meshElements & MeshElement::MeshNorm)
				{
					FbxGeometryElementNormal* normElement = mesh->GetElementNormal();

					if (normElement != nullptr)
					{
						int normIndex = vertIndex;

						if (normElement->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
							normIndex = pointIndex;

						if (normElement->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
							normIndex = normElement->GetIndexArray()[normIndex];

						FbxVector4 norm = normElement->GetDirectArray()[normIndex];

						vert.norm[0] = (float)norm[0];
						vert.norm[1] = (float)norm[1];
						vert.norm[2] = (float)norm[2];
					}
				}

				// get vertex color
				if (_meshElements & MeshElement::MeshColor)
				{
					FbxGeometryElementVertexColor* colorElement = mesh->GetElementVertexColor();

					if (colorElement != nullptr)
					{
						int colorIndex = vertIndex;

						if (colorElement->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
							colorIndex = pointIndex;

						if (colorElement->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
							colorIndex = colorElement->GetIndexArray()[colorIndex];

						FbxColor color = colorElement->GetDirectArray()[colorIndex];

						vert.color[0] = (float)color[0];
						vert.color[1] = (float)color[1];
						vert.color[2] = (float)color[2];
						vert.color[3] = (float)color[3];
					}
				}

				// get vertex UV coord
				if (_meshElements & MeshElement::MeshTex)
				{
					FbxGeometryElementUV* uvElement = mesh->GetElementUV();

					if (uvElement != nullptr)
					{
						int uvIndex = vertIndex;

						if (uvElement->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
							uvIndex = pointIndex;

						if (uvElement->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
							uvIndex = uvElement->GetIndexArray()[uvIndex];

						FbxVector2 tex = uvElement->GetDirectArray()[uvIndex];

						vert.tex[0] = tex[0];
						vert.tex[1] = 1.0f - tex[1];
					}
				}

				// add vert to list
				rawVerts.push_back(vert);
			}
		}
		// -- /extract vertices from mesh --

		// -- index data --
		std::vector<simple_vertex_s> uniqueVerts;
		std::vector<uint32_t> outInds;

		// test all verts
		for (int i = 0; i < rawVerts.size(); i++)
		{
			simple_vertex_s vert = rawVerts[i];

			bool unique = true;
			uint32_t index = 0;

			// compare current vert to unique verts
			for (int v = 0; v < uniqueVerts.size(); v++)
			{
				// if vert matches a unique vert, store index and move on
				if (vert == uniqueVerts[v])
				{
					unique = false;
					index = v;
					break;
				}
			}

			// if vert is unique, get new index and add to unique vert list
			if (unique)
			{
				index = uniqueVerts.size();
				uniqueVerts.push_back(vert);
			}

			outInds.push_back(index);
		}
		// -- /index data --

		// -- write to file --
		// open or create output file for writing
		std::fstream fout = std::fstream(_outputFilepath, std::ios_base::out | std::ios_base::binary);

		// verify file is open
		if (fout.is_open())
		{
			uint32_t numVerts = uniqueVerts.size();
			uint32_t numInds = outInds.size();

			// write data to file with format:
			//   uint32_t											: number of vertices
			//   { float3, float3, float4, float2 }[numVerts]		: vertex data
			//   uint32_t											: number of indices
			//   uint32_t[numInds]									: index data
			fout.write((const char*)&numVerts, sizeof(numVerts));
			fout.write((const char*)&uniqueVerts[0], numVerts * sizeof(simple_vertex_s));
			fout.write((const char*)&numInds, sizeof(numInds));
			fout.write((const char*)&outInds[0], numInds * sizeof(uint32_t));

			result = 0;
		}
		// -- /write to file --


		float reductionAmount = (rawVerts.size() - uniqueVerts.size()) / (rawVerts.size() * 1.0f) * 100;
		uint32_t numBytes = (uniqueVerts.size() * sizeof(simple_vertex_s)) + (outInds.size() * sizeof(uint32_t)) + (2 * sizeof(uint32_t));


		std::cout
			<< "Raw vertex count : " << rawVerts.size() << std::endl
			<< "Unique vertex count : " << uniqueVerts.size() << std::endl
			<< "Reduction : " << reductionAmount << "%" << std::endl
			<< "Index count : " << outInds.size() << std::endl
			<< "Wrote " << numBytes << " bytes to file" << std::endl
			<< std::endl;


		return result;
	}

	int ExtractMaterial(const FbxScene* _scene, const char* _outputFilepath, uint32_t _matNum, int32_t _matElements)
	{
		int result = -1;

		std::vector<simple_material_s> materials;
		std::vector<filepath_t> filepaths;

		FbxScene* scene = (FbxScene*)_scene;
		FbxSurfaceMaterial* mat = nullptr;

		// -- extract material from scene --

		// if material number is specified and scene has a material with that index
		if (_matNum > 0 && scene->GetGeometryCount() > _matNum)
			mat = scene->GetMaterial(_matNum);
		// if material number is unspecified or scene does not have a material with that index
		else
			mat = scene->GetMaterial(0);

		simple_material_s outMat;

		// skip non-standard materials
		if (mat->Is<FbxSurfaceLambert>() == false)
			return result;

		FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)mat;

		if (_matElements & MaterialElement::MatDiffuse)
		{
			FbxDouble3 diffuseColor = lambert->Diffuse.Get();
			FbxDouble diffuseFactor = lambert->DiffuseFactor.Get();

			outMat[simple_material_s::eComponent::DIFFUSE].value[0] = (float)diffuseColor[0];
			outMat[simple_material_s::eComponent::DIFFUSE].value[1] = (float)diffuseColor[1];
			outMat[simple_material_s::eComponent::DIFFUSE].value[2] = (float)diffuseColor[2];

			outMat[simple_material_s::eComponent::DIFFUSE].factor = (float)diffuseFactor;

			FbxFileTexture* fileTexture = lambert->Diffuse.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				outMat[simple_material_s::eComponent::DIFFUSE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_matElements & MaterialElement::MatEmissive)
		{
			FbxDouble3 emissiveColor = lambert->Emissive.Get();
			FbxDouble emissiveFactor = lambert->EmissiveFactor.Get();

			outMat[simple_material_s::eComponent::EMISSIVE].value[0] = (float)emissiveColor[0];
			outMat[simple_material_s::eComponent::EMISSIVE].value[1] = (float)emissiveColor[1];
			outMat[simple_material_s::eComponent::EMISSIVE].value[2] = (float)emissiveColor[2];

			outMat[simple_material_s::eComponent::EMISSIVE].factor = (float)emissiveFactor;

			FbxFileTexture* fileTexture = lambert->Emissive.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				outMat[simple_material_s::eComponent::EMISSIVE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_matElements & MaterialElement::MatSpecular)
		{
			if (mat->Is<FbxSurfacePhong>())
			{
				FbxSurfacePhong* phong = (FbxSurfacePhong*)mat;

				FbxDouble3 specularColor = phong->Specular.Get();
				FbxDouble specularFactor = phong->SpecularFactor.Get();

				outMat[simple_material_s::eComponent::SPECULAR].value[0] = (float)specularColor[0];
				outMat[simple_material_s::eComponent::SPECULAR].value[1] = (float)specularColor[1];
				outMat[simple_material_s::eComponent::SPECULAR].value[2] = (float)specularColor[2];

				outMat[simple_material_s::eComponent::SPECULAR].factor = (float)specularFactor;

				FbxFileTexture* fileTexture = phong->Specular.GetSrcObject<FbxFileTexture>();
				if (fileTexture != nullptr)
				{
					const char* filename = fileTexture->GetRelativeFileName();
					filepath_t filepath;
					strcpy_s(filepath.data(), filepath.max_size(), filename);
					outMat[simple_material_s::eComponent::SPECULAR].input = filepaths.size();
					filepaths.push_back(filepath);
				}
			}
		}

		if (_matElements & MaterialElement::MatNormalMap)
		{
			FbxDouble3 normalColor = lambert->NormalMap.Get();

			outMat[simple_material_s::eComponent::NORMALMAP].value[0] = (float)normalColor[0];
			outMat[simple_material_s::eComponent::NORMALMAP].value[1] = (float)normalColor[1];
			outMat[simple_material_s::eComponent::NORMALMAP].value[2] = (float)normalColor[2];

			outMat[simple_material_s::eComponent::NORMALMAP].factor = 1.0f;

			FbxFileTexture* fileTexture = lambert->NormalMap.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				outMat[simple_material_s::eComponent::NORMALMAP].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		materials.push_back(outMat);
		// -- /extract material from scene --

		// -- write to file --
		// open or create output file for writing
		std::fstream fout = std::fstream(_outputFilepath, std::ios_base::out | std::ios_base::binary);

		// verify file is open
		if (fout.is_open())
		{
			uint32_t numMats = materials.size();
			uint32_t numPaths = filepaths.size();

			// write data to file with format:
			//   uint32_t												: number of materials
			//   { float3, float, uint64_t }[compType::COUNT][numMats]	: material data
			//   uint32_t												: number of filepaths
			//   filepath_t[numPaths]									: filepath data
			fout.write((const char*)&numMats, sizeof(numMats));
			fout.write((const char*)&materials[0], numMats * sizeof(simple_material_s));
			fout.write((const char*)&numPaths, sizeof(numPaths));
			fout.write((const char*)&filepaths[0], numPaths * sizeof(filepath_t));

			result = 0;
		}
		// -- /write to file --
		

		uint32_t numBytes = (materials.size() * sizeof(simple_material_s)) + (filepaths.size() * sizeof(filepath_t)) + (2 * sizeof(uint32_t));
		

		std::cout
			<< "Number of components exported : " << filepaths.size() << std::endl
			<< "Filepaths : " << std::endl;
		for (uint8_t i = 0; i < filepaths.size(); i++)
		{
			char* path = new char[260];
			memcpy(&path[0], &filepaths[i][0], 260);
			std::cout << path << std::endl;
		}
		std::cout
			<< "Wrote " << numBytes<< " bytes to file" << std::endl
			<< std::endl;


		return result;
	}
}