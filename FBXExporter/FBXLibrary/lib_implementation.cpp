#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include "lib_debug.h"
#include "lib_interface.h"
#include "lib_utility.h"


namespace FBXLibrary
{
#pragma region Private Helper Functions
	// Searches a scene for a mesh with the name specified, or returns the first mesh if no name is specified
	// Returns a result code
	int GetFbxMeshFromScene(FbxMesh*& _mesh_p, FbxScene* _scene_p, const char* _meshName = nullptr)
	{
		int result = FAIL;
		FbxGeometry* geo_p = nullptr;

		// search scene for mesh name
		if (_meshName != nullptr)
			// search scene for mesh name
			for (int i = 0; i < _scene_p->GetGeometryCount(); i++)
			{
				geo_p = _scene_p->GetGeometry(i);

				// if geometry is a mesh and name matches, store mesh
				if (geo_p->GetAttributeType() == FbxNodeAttribute::eMesh && geo_p->GetName() == _meshName)
				{
					_mesh_p = (FbxMesh*)geo_p;
					break;
				}
			}
		// if no mesh name is specified, use first mesh
		else
			// find first mesh in scene
			for (int i = 0; i < _scene_p->GetGeometryCount(); i++)
			{
				geo_p = _scene_p->GetGeometry(i);

				// if geometry is a mesh, store mesh
				if (geo_p->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					_mesh_p = (FbxMesh*)geo_p;
					break;
				}
			}

		if (_mesh_p != nullptr)
			result = SUCCESS;

		return result;
	}

	// Extracts raw vertices (control points) from a mesh
	// Returns a result code
	int ExtractVertsFromFbxMesh(std::vector<SIMPLE_VERTEX>& _verts, FbxMesh* _mesh_p, uint32_t _meshElements)
	{
		int result = FAIL;

		// verify mesh exists
		if (_mesh_p != nullptr)
		{
			// number of polygons in mesh
			int polygonCount = _mesh_p->GetPolygonCount();

			// array of FBX polygon vertices (equivalent to vertex indices)
			int* vertexIndices = _mesh_p->GetPolygonVertices();

			// array of FBX control points (equivalent to vertices)
			const FbxVector4* controlPoints_p = _mesh_p->GetControlPoints();


			// for each polygon in mesh
			for (int i = 0; i < polygonCount; i++)
			{
				// for each vertex in polygon
				for (int v = 0; v < 3; v++)
				{
					// vertex to read data into
					SIMPLE_VERTEX vert;

					// position of vertex's index in index list
					int indexNum = i * 3 + v;

					// vertex's index
					int vertNum = vertexIndices[indexNum];

					// get vertex elements
					GetVertexElements(vert, controlPoints_p, _mesh_p, indexNum, vertNum, _meshElements);

					// add vert to list
					_verts.push_back(vert);
				}
			}

			// verify vertices were extracted from mesh
			if (_verts.size() > 0)
				result = SUCCESS;
		}

		return result;
	}

	void GetVertexPosition(SIMPLE_VERTEX& _vert, const FbxVector4* _controlPoints_p, int _vertNum)
	{
		// get position from vertex (control point)
		FbxVector4 pos = _controlPoints_p[_vertNum];

		// store data
		_vert.pos[0] = (float)pos[0];
		_vert.pos[1] = (float)pos[1];
		_vert.pos[2] = (float)pos[2];
	}
	void GetVertexNormal(SIMPLE_VERTEX& _vert, FbxMesh* _mesh_p, int _indexNum, int _vertNum)
	{
		// get normal element from mesh
		FbxGeometryElementNormal* normElement = _mesh_p->GetElementNormal();

		// verify normal element exists
		if (normElement != nullptr)
		{
			// determine normal index
			int normIndex = _indexNum;
			if (normElement->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
				normIndex = _vertNum;
			if (normElement->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
				normIndex = normElement->GetIndexArray()[normIndex];

			// get vertex normal from normal element
			FbxVector4 norm = normElement->GetDirectArray()[normIndex];

			// store data
			_vert.norm[0] = (float)norm[0];
			_vert.norm[1] = (float)norm[1];
			_vert.norm[2] = (float)norm[2];
		}
	}
	void GetVertexColor(SIMPLE_VERTEX& _vert, FbxMesh* _mesh_p, int _indexNum, int _vertNum)
	{
		// get color element from mesh
		FbxGeometryElementVertexColor* colorElement = _mesh_p->GetElementVertexColor();

		// verify color element exists
		if (colorElement != nullptr)
		{
			// determine color index
			int colorIndex = _indexNum;
			if (colorElement->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
				colorIndex = _vertNum;
			if (colorElement->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
				colorIndex = colorElement->GetIndexArray()[colorIndex];

			// get vertex color from color element
			FbxColor color = colorElement->GetDirectArray()[colorIndex];

			// store data
			_vert.color[0] = (float)color[0];
			_vert.color[1] = (float)color[1];
			_vert.color[2] = (float)color[2];
			_vert.color[3] = (float)color[3];
		}
	}
	void GetVertexUV(SIMPLE_VERTEX& _vert, FbxMesh* _mesh_p, int _indexNum, int _vertNum)
	{
		// get UV element from mesh
		FbxGeometryElementUV* uvElement = _mesh_p->GetElementUV();

		// verify UV element exists
		if (uvElement != nullptr)
		{
			// determine UV index
			int uvIndex = _indexNum;
			if (uvElement->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
				uvIndex = _vertNum;
			if (uvElement->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
				uvIndex = uvElement->GetIndexArray()[uvIndex];

			// get vertex texcoord from UV element
			FbxVector2 tex = uvElement->GetDirectArray()[uvIndex];

			// store data
			_vert.tex[0] = (float)tex[0];
			_vert.tex[1] = (float)(1.0f - tex[1]);
		}
	}

	void GetVertexElements(SIMPLE_VERTEX& _vert, const FbxVector4* _controlPoints_p, const FbxMesh* _mesh_p, int _indexNum, int _vertNum, uint32_t _meshElements)
	{
		// if position flag is set, get position
		if (_meshElements & MESH_ELEMENT::POSITION)
			GetVertexPosition(_vert, _controlPoints_p, _vertNum);

		// if normal flag is set, get normal
		if (_meshElements & MESH_ELEMENT::NORMAL)
			GetVertexNormal(_vert, (FbxMesh*)_mesh_p, _indexNum, _vertNum);

		// if color flag is set, get color
		if (_meshElements & MESH_ELEMENT::COLOR)
			GetVertexColor(_vert, (FbxMesh*)_mesh_p, _indexNum, _vertNum);

		// if texcoord flag is set, get UV
		if (_meshElements & MESH_ELEMENT::TEXCOORD)
			GetVertexUV(_vert, (FbxMesh*)_mesh_p, _indexNum, _vertNum);
	}

	// Compares vertices in a list and generates a list of unique verts and a list of indices
	// Returns a result code
	int CompactifyVerts(std::vector<SIMPLE_VERTEX>& _verts_raw, std::vector<SIMPLE_VERTEX>& _verts_unique, std::vector<uint32_t> _indices)
	{
		int result = FAIL;

		// for all raw vertices
		for (int i = 0; i < _verts_raw.size(); i++)
		{
			// get vertex number i
			SIMPLE_VERTEX vert = _verts_raw[i];

			// index to add to index list
			uint32_t index = 0;

			bool isUnique = true;

			// compare current vert to all unique vertices
			for (int v = 0; v < _verts_unique.size(); v++)
			{
				// if vert matches a unique vert, mark as not unique and store index of matching vert
				if (vert == _verts_unique[v])
				{
					isUnique = false;
					index = v;
					break;
				}
			}

			// if vert is unique, store new index and add vert to unique vert list
			if (isUnique)
			{
				index = (uint32_t)_verts_unique.size();
				_verts_unique.push_back(vert);
			}

			// store index
			_indices.push_back(index);
		}

		// verify verts and inds were generated
		if (_verts_unique.size() > 0 && _indices.size() > 0)
			result = SUCCESS;

		return result;
	}
#pragma endregion

#pragma region Utility Functions
	int CreateAndImport(const char* _fbxFilepath, FbxManager*& _manager, FbxScene*& _scene)
	{
		// sdk manager handles memory management
		_manager = FbxManager::Create();

		// return FAIL if manager was not created
		if (_manager == nullptr)
			return FAIL;

		// create IO settings object
		FbxIOSettings* ios = FbxIOSettings::Create(_manager, "");
		_manager->SetIOSettings(ios);

		// create importer using sdk manager
		FbxImporter* importer = FbxImporter::Create(_manager, "");

		// initialize importer, or display error info and return FAIL if initialization failed
		if (!importer->Initialize(_fbxFilepath, -1, _manager->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
			return FAIL;
		}

		// create new scene to populate with imported file
		_scene = FbxScene::Create(_manager, "imported_scene");

		// import contents of file into scene
		importer->Import(_scene);

		importer->Destroy();

		return SUCCESS;
	}

	SIMPLE_MATRIX FbxAMatrixToSimpleMatrix(FbxAMatrix _m)
	{
		return
		{
			(float)_m.Get(0, 0),
			(float)_m.Get(0, 1),
			(float)_m.Get(0, 2),
			(float)_m.Get(0, 3),

			(float)_m.Get(1, 0),
			(float)_m.Get(1, 1),
			(float)_m.Get(1, 2),
			(float)_m.Get(1, 3),

			(float)_m.Get(2, 0),
			(float)_m.Get(2, 1),
			(float)_m.Get(2, 2),
			(float)_m.Get(2, 3),

			(float)_m.Get(3, 0),
			(float)_m.Get(3, 1),
			(float)_m.Get(3, 2),
			(float)_m.Get(3, 3)
		};
	}

	int ExtractFbxMesh(const FbxScene* _scene_p, const char* _outputFilepath, SIMPLE_MESH& _simple_mesh, const char* _meshName, int32_t _meshElements)
	{
		int result = FAIL;

		FbxScene* scene_p = (FbxScene*)_scene_p;
		FbxMesh* mesh_p = nullptr;

		// uncompactified vertex data
		std::vector<SIMPLE_VERTEX> verts_raw;

		// find mesh with specified name, or use first mesh if none is specified
		result = GetFbxMeshFromScene(mesh_p, scene_p, _meshName);
		if (FAILED(result))
			return result;

		// extract raw vertices from mesh
		result = ExtractVertsFromFbxMesh(verts_raw, mesh_p, _meshElements);
		if (FAILED(result))
			return result;

		// compactify data
		result = CompactifyVerts(verts_raw, _simple_mesh.vertices, _simple_mesh.indices);
		if (FAILED(result))
			return result;
		
		// store vertex and index count
		_simple_mesh.vertex_count = _simple_mesh.vertices.size();
		_simple_mesh.index_count = _simple_mesh.indices.size();

		return result;
	}
	int ExtractFbxMaterial(const FbxScene* _scene, const char* _outputFilepath, SIMPLE_MATERIAL_LIST& _simple_material_list, uint32_t _matNum, int32_t _matElements)
	{
		int result = -1;

		std::vector<SIMPLE_MATERIAL> materials;
		std::vector<filepath_t> filepaths;

		FbxScene* scene = (FbxScene*)_scene;
		FbxSurfaceMaterial* mat = nullptr;


		// -- extract material from scene --

		// if material number is specified and scene has a material with that index
		if (_matNum > 0 && scene->GetGeometryCount() > (int)_matNum)
			mat = scene->GetMaterial(_matNum);
		// if material number is unspecified or scene does not have a material with that index
		else
			mat = scene->GetMaterial(0);

		SIMPLE_MATERIAL outMat;

		// skip non-standard materials
		if (mat->Is<FbxSurfaceLambert>() == false)
			return result;

		FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)mat;

		if (_matElements & MATERIAL_ELEMENT::DIFFUSE)
		{
			FbxDouble3 diffuseColor = lambert->Diffuse.Get();
			FbxDouble diffuseFactor = lambert->DiffuseFactor.Get();

			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[0] = (float)diffuseColor[0];
			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[1] = (float)diffuseColor[1];
			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[2] = (float)diffuseColor[2];

			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].factor = (float)diffuseFactor;

			FbxFileTexture* fileTexture = lambert->Diffuse.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_matElements & MATERIAL_ELEMENT::EMISSIVE)
		{
			FbxDouble3 emissiveColor = lambert->Emissive.Get();
			FbxDouble emissiveFactor = lambert->EmissiveFactor.Get();

			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[0] = (float)emissiveColor[0];
			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[1] = (float)emissiveColor[1];
			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[2] = (float)emissiveColor[2];

			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].factor = (float)emissiveFactor;

			FbxFileTexture* fileTexture = lambert->Emissive.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_matElements & MATERIAL_ELEMENT::SPECULAR)
		{
			if (mat->Is<FbxSurfacePhong>())
			{
				FbxSurfacePhong* phong = (FbxSurfacePhong*)mat;

				FbxDouble3 specularColor = phong->Specular.Get();
				FbxDouble specularFactor = phong->SpecularFactor.Get();

				outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[0] = (float)specularColor[0];
				outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[1] = (float)specularColor[1];
				outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[2] = (float)specularColor[2];

				outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].factor = (float)specularFactor;

				FbxFileTexture* fileTexture = phong->Specular.GetSrcObject<FbxFileTexture>();
				if (fileTexture != nullptr)
				{
					const char* filename = fileTexture->GetRelativeFileName();
					filepath_t filepath;
					strcpy_s(filepath.data(), filepath.max_size(), filename);
					outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].input = filepaths.size();
					filepaths.push_back(filepath);
				}
			}
		}

		if (_matElements & MATERIAL_ELEMENT::NORMALMAP)
		{
			FbxDouble3 normalColor = lambert->NormalMap.Get();

			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[0] = (float)normalColor[0];
			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[1] = (float)normalColor[1];
			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[2] = (float)normalColor[2];

			outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].factor = 1.0f;

			FbxFileTexture* fileTexture = lambert->NormalMap.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				outMat[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].input = filepaths.size();
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
			uint32_t numMats = (uint32_t)materials.size();
			uint32_t numPaths = (uint32_t)filepaths.size();
			uint32_t numBytes = sizeof(numMats) + sizeof(numPaths) + (numMats * sizeof(SIMPLE_MATERIAL)) + (numPaths * sizeof(filepath_t));

			// write data to file with format:
			//   uint32_t												: number of materials
			//   { float3, float, uint64_t }[compType::COUNT][numMats]	: material data
			//   uint32_t												: number of filepaths
			//   filepath_t[numPaths]									: filepath data
			fout.write((const char*)&numMats, sizeof(numMats));
			fout.write((const char*)&materials[0], numMats * sizeof(SIMPLE_MATERIAL));
			fout.write((const char*)&numPaths, sizeof(numPaths));
			fout.write((const char*)&filepaths[0], numPaths * sizeof(filepath_t));


			std::cout
				<< "Number of components exported : " << filepaths.size() << std::endl
				<< "Filepaths : " << std::endl;
			for (uint8_t i = 0; i < filepaths.size(); i++)
			{
				char path[260];
				memcpy(path, &filepaths[i][0], 260);
				std::cout << path << std::endl;
			}
			std::cout
				<< "Wrote " << numBytes << " bytes to file" << std::endl
				<< std::endl;


			result = 0;
		}

		// -- /write to file --


		return result;
	}
	int ExtractFbxAnimation(const FbxScene* _scene, const char* _outputFilepath, SIMPLE_ANIM_CLIP& _simple_anim_clip, uint32_t _animElements)
	{
		int result = -1;

		FbxScene* scene = (FbxScene*)_scene;
		FbxMesh* mesh = nullptr;


		// get pose count from scene
		uint32_t poseCount = scene->GetPoseCount();
		FbxPose* bindPose = nullptr;


		// -- extract bind pose from scene --

		for (uint32_t i = 0; i < poseCount; i++)
		{
			FbxPose* pose = scene->GetPose(i);
			if (pose->IsBindPose())
			{
				bindPose = pose;
				break;
			}
		}

		// -- /extract bind pose from scene --


		// -- get skeleton root from bind pose --

		uint32_t nodeCount = 0;
		FbxNode* rootNode = nullptr;
		FbxSkeleton* rootSkeleton = nullptr;

		if (bindPose == nullptr)
			return result;
		else
		{
			nodeCount = bindPose->GetCount();
			FbxNode* node = nullptr;
			FbxSkeleton* skeleton = nullptr;

			for (uint32_t i = 0; i < nodeCount; i++)
			{
				node = bindPose->GetNode(i);
				if (node != nullptr)
				{
					skeleton = node->GetSkeleton();
					if (skeleton != nullptr && skeleton->IsSkeletonRoot())
					{
						rootNode = node;
						rootSkeleton = skeleton;
						break;
					}
				}
			}
		}

		// -- /get skeleton root from bind pose --


		// -- create list of joints from skeleton root --

		std::vector<FBX_JOINT> joints_fbx;

		FBX_JOINT rootJoint = { rootNode, -1 };
		joints_fbx.push_back(rootJoint);

		for (uint32_t i = 0; i < joints_fbx.size(); i++)
		{
			FbxNode* node = joints_fbx[i].node;

			for (int32_t c = 0; c < node->GetChildCount(); c++)
			{
				FbxNode* childNode = nullptr;
				FbxSkeleton* childSkeleton = nullptr;

				childNode = node->GetChild(c);

				if (childNode != nullptr)
					childSkeleton = childNode->GetSkeleton();

				if (childSkeleton != nullptr)
				{
					FBX_JOINT childJoint = { childNode, (int)i };
					joints_fbx.push_back(childJoint);
				}
			}
		}

		// -- /create list of joints from skeleton root --


		// -- convert bind pose joint data --

		std::vector<SIMPLE_JOINT> joints_out;

		for (uint32_t i = 0; i < joints_fbx.size(); i++)
		{
			FbxAMatrix transform = joints_fbx[i].node->EvaluateGlobalTransform();

			SIMPLE_JOINT joint = { FbxAMatrixToSimpleMatrix(transform), joints_fbx[i].parent_index };

			joints_out.push_back(joint);
		}

		// -- /convert bind pose joint data --


		// -- get animation data from scene --

		FbxAnimStack* animStack = scene->GetCurrentAnimationStack();
		SIMPLE_ANIM_CLIP animClip;

		if (animStack == nullptr)
			return result;
		else
		{
			FbxTime::EMode mode = FbxTime::EMode::eFrames30;

			// get animation duration and frame count
			FbxTime animDuration = animStack->GetLocalTimeSpan().GetDuration();
			int64_t frameCount = animDuration.GetFrameCount(mode);

			// store duration in seconds
			animClip.duration = animDuration.GetSecondDouble();

			for (int64_t i = 1; i < frameCount; i++) // starts at 1 to skip bind post at frame 0
			{
				SIMPLE_ANIM_FRAME keyframe;

				// get keytime for current frame
				FbxTime keytime;
				keytime.SetFrame(i, mode);

				// store keytime in seconds
				keyframe.time = keytime.GetSecondDouble();

				// get node transforms for current frame
				for (uint32_t n = 0; n < joints_fbx.size(); n++)
					keyframe.transforms.push_back({ FbxAMatrixToSimpleMatrix(joints_fbx[n].node->EvaluateGlobalTransform(keytime)) });

				animClip.frames.push_back(keyframe);
			}
		}

		// -- /get animation data from scene --


		// -- write to file --

		// open or create output file for writing
		std::fstream fout = std::fstream(_outputFilepath, std::ios_base::out | std::ios_base::binary);

		// verify file is open
		if (fout.is_open())
		{
			uint32_t numJoints = (uint32_t)joints_out.size();
			uint32_t frameSize = sizeof(double) + (numJoints * sizeof(SIMPLE_MATRIX));
			uint32_t numFrames = (uint32_t)animClip.frames.size();
			uint32_t numBytes = sizeof(numJoints) + sizeof(numFrames) + (numJoints * sizeof(SIMPLE_JOINT)) + sizeof(animClip.duration) + (numFrames * frameSize);

			// write bind pose to file with format:
			//   uint32_t										: number of joints
			//   { float[16], int }[numJoints]					: joint data
			fout.write((char*)&numJoints, sizeof(numJoints));
			fout.write((char*)&joints_out[0], numJoints * sizeof(SIMPLE_JOINT));

			// write animation clip to file with format:
			//   double											: animation duration in seconds
			//   uint32_t										: byte length of each frame
			//   uint32_t										: number of frames
			//   { double, float[16][numJoints] }[numFrames]	: frame data
			fout.write((char*)&animClip.duration, sizeof(animClip.duration));
			fout.write((char*)&frameSize, sizeof(frameSize));
			fout.write((char*)&numFrames, sizeof(numFrames));
			for (uint32_t i = 0; i < numFrames; i++)
			{
				fout.write((char*)&animClip.frames[i].time, sizeof(SIMPLE_ANIM_FRAME::time));
				fout.write((char*)&animClip.frames[i].transforms[0], frameSize - sizeof(SIMPLE_ANIM_FRAME::time));
			}


			std::cout
				<< "Joint count : " << numJoints << std::endl
				<< "Duration : " << animClip.duration << std::endl
				<< "Frame byte length : " << frameSize << std::endl
				<< "Frame count : " << numFrames << std::endl
				<< "Wrote " << numBytes << " bytes to file" << std::endl
				<< std::endl;


			result = 0;
		}

		// -- /write to file --


		joints_fbx.clear();
		joints_out.clear();


		return result;
	}
#pragma endregion

#pragma region Interface Functions
	int GetScenePolyCount(const char* _fbxFilepath)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbxFilepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// verify scene import
		if (scene_p == nullptr)
			return FAIL;

		// get geometry count from scene
		int geoCount = scene_p->GetGeometryCount();

		// set initial count
		result = 0;

		// iterate through scene geometries
		for (int i = 0; i < geoCount; i++)
		{
			FbxGeometry* geo = scene_p->GetGeometry(i);

			// skip non-mesh geometries
			if (geo->GetAttributeType() != FbxNodeAttribute::eMesh)
				continue;

			// add mesh's polygon count to result
			FbxMesh* mesh = (FbxMesh*)geo;
			result += mesh->GetPolygonCount();
		}

		// set result to error if no polygons were counted
		if (result < 1)
			result = FAIL;

		manager_p->Destroy();

		return result;
	}
	int ExtractMesh(const char* _fbxFilepath, const char* _outputFilepath, SIMPLE_MESH& _simple_mesh, const char* _meshName, int32_t _meshElements)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbxFilepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// verify scene import
		if (scene_p == nullptr)
			return FAIL;

		// extract mesh from scene
		result = ExtractFbxMesh(scene_p, _outputFilepath, _simple_mesh, _meshName, _meshElements);

		manager_p->Destroy();

		return result;
	}

	int GetSceneMaterialCount(const char* _fbxFilepath)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbxFilepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// verify scene import
		if (scene_p == nullptr)
			return FAIL;

		// set result default
		result = FAIL;

		// get material count from scene
		result = scene_p->GetMaterialCount();

		manager_p->Destroy();

		return result;
	}
	int ExtractMaterial(const char* _fbxFilepath, const char* _outputFilepath, SIMPLE_MATERIAL_LIST& _simple_material_list, uint32_t _matNum, int32_t _matElements)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbxFilepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// verify scene import
		if (scene_p == nullptr)
			return FAIL;

		// extract material from scene
		result = ExtractFbxMaterial(scene_p, _outputFilepath, _simple_material_list, _matNum, _matElements);

		manager_p->Destroy();

		return result;
	}

	int GetScenePoseCount(const char* _fbxFilepath)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbxFilepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// verify scene import
		if (scene_p == nullptr)
			return FAIL;

		// set result default
		result = FAIL;

		// get pose count from scene
		result = scene_p->GetPoseCount();

		manager_p->Destroy();

		return result;
	}
	int ExtractAnimation(const char* _fbxFilepath, const char* _outputFilepath, SIMPLE_ANIM_CLIP& _simple_anim_clip, uint32_t _animElements)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbxFilepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// verify scene import
		if (scene_p == nullptr)
			return FAIL;

		// extract animation from scene
		result = ExtractFbxAnimation(scene_p, _outputFilepath, _simple_anim_clip, _animElements);

		manager_p->Destroy();

		return result;
	}
#pragma endregion

}
