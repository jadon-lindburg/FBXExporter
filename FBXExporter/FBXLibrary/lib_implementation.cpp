#include <array>
#include <fstream>
#include <iostream>

#include "lib_debug.h"
#include "lib_interface.h"
#include "lib_utility.h"


namespace FBXLibrary
{
#pragma region Private Helper Functions
	int GetFbxMeshFromScene(FbxMesh*& _fbx_mesh_p, FbxScene* _fbx_scene_p, const char* _meshName = nullptr)
	{
		int result = FAIL;
		FbxGeometry* fbx_geometry_p = nullptr;

		// search scene for mesh name
		if (_meshName != nullptr)
			// search scene for mesh name
			for (int i = 0; i < _fbx_scene_p->GetGeometryCount(); i++)
			{
				fbx_geometry_p = _fbx_scene_p->GetGeometry(i);

				// if geometry is a mesh and name matches, store mesh
				if (fbx_geometry_p->GetAttributeType() == FbxNodeAttribute::eMesh && fbx_geometry_p->GetName() == _meshName)
				{
					_fbx_mesh_p = (FbxMesh*)fbx_geometry_p;
					break;
				}
			}
		// if no mesh name is specified, use first mesh
		else
			// find first mesh in scene
			for (int i = 0; i < _fbx_scene_p->GetGeometryCount(); i++)
			{
				fbx_geometry_p = _fbx_scene_p->GetGeometry(i);

				// if geometry is a mesh, store mesh
				if (fbx_geometry_p->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					_fbx_mesh_p = (FbxMesh*)fbx_geometry_p;
					break;
				}
			}

		if (_fbx_mesh_p != nullptr)
			result = SUCCESS;

		return result;
	}

	void GetVertexPosition(SIMPLE_VERTEX& _vert, const FbxVector4* _fbx_controlPoints_p, int _vertNum)
	{
		// get position from vertex (control point)
		FbxVector4 pos = _fbx_controlPoints_p[_vertNum];

		// store data
		_vert.pos[0] = (float)pos[0];
		_vert.pos[1] = (float)pos[1];
		_vert.pos[2] = (float)pos[2];
	}
	void GetVertexNormal(SIMPLE_VERTEX& _vert, FbxMesh* _fbx_mesh_p, int _indexNum, int _vertNum)
	{
		// get normal element from mesh
		FbxGeometryElementNormal* normElement = _fbx_mesh_p->GetElementNormal();

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
	void GetVertexColor(SIMPLE_VERTEX& _vert, FbxMesh* _fbx_mesh_p, int _indexNum, int _vertNum)
	{
		// get color element from mesh
		FbxGeometryElementVertexColor* colorElement = _fbx_mesh_p->GetElementVertexColor();

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
	void GetVertexUV(SIMPLE_VERTEX& _vert, FbxMesh* _fbx_mesh_p, int _indexNum, int _vertNum)
	{
		// get UV element from mesh
		FbxGeometryElementUV* uvElement = _fbx_mesh_p->GetElementUV();

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

	void GetVertexElements(SIMPLE_VERTEX& _vert, const FbxVector4* _fbx_controlPoints_p,
		const FbxMesh* _fbx_mesh_p, int _indexNum, int _vertNum, uint32_t _elementOptions)
	{
		// if position flag is set, get position
		if (_elementOptions & MESH_ELEMENT::POSITION)
			GetVertexPosition(_vert, _fbx_controlPoints_p, _vertNum);

		// if normal flag is set, get normal
		if (_elementOptions & MESH_ELEMENT::NORMAL)
			GetVertexNormal(_vert, (FbxMesh*)_fbx_mesh_p, _indexNum, _vertNum);

		// if color flag is set, get color
		if (_elementOptions & MESH_ELEMENT::COLOR)
			GetVertexColor(_vert, (FbxMesh*)_fbx_mesh_p, _indexNum, _vertNum);

		// if texcoord flag is set, get UV
		if (_elementOptions & MESH_ELEMENT::TEXCOORD)
			GetVertexUV(_vert, (FbxMesh*)_fbx_mesh_p, _indexNum, _vertNum);
	}

	int ExtractVertsFromFbxMesh(std::vector<SIMPLE_VERTEX>& _verts, FbxMesh* _fbx_mesh_p, uint32_t _elementOptions)
	{
		int result = FAIL;

		// verify mesh exists
		if (_fbx_mesh_p != nullptr)
		{
			// number of polygons in mesh
			int polygonCount = _fbx_mesh_p->GetPolygonCount();

			// array of FBX polygon vertices (equivalent to vertex indices)
			int* vertexIndices = _fbx_mesh_p->GetPolygonVertices();

			// array of FBX control points (equivalent to vertices)
			const FbxVector4* controlPoints_p = _fbx_mesh_p->GetControlPoints();


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
					GetVertexElements(vert, controlPoints_p, _fbx_mesh_p, indexNum, vertNum, _elementOptions);

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
	int CreateAndImport(const char* _fbx_filepath, FbxManager*& _fbx_manager_p, FbxScene*& _fbx_scene_p)
	{
		// sdk manager handles memory management
		_fbx_manager_p = FbxManager::Create();

		// return FAIL if manager was not created
		if (_fbx_manager_p == nullptr)
			return FAIL;

		// create IO settings object
		FbxIOSettings* ios = FbxIOSettings::Create(_fbx_manager_p, "");
		_fbx_manager_p->SetIOSettings(ios);

		// create importer using sdk manager
		FbxImporter* importer = FbxImporter::Create(_fbx_manager_p, "");

		// initialize importer, or display error info and return FAIL if initialization failed
		if (!importer->Initialize(_fbx_filepath, -1, _fbx_manager_p->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
			return FAIL;
		}

		// create new scene to populate with imported file
		_fbx_scene_p = FbxScene::Create(_fbx_manager_p, "imported_scene");

		// import contents of file into scene
		importer->Import(_fbx_scene_p);

		importer->Destroy();

		// verify scene was imported
		if (_fbx_scene_p == nullptr)
			return FAIL;

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

	int ExtractFbxMesh(const FbxScene* _fbx_scene_p, SIMPLE_MESH& _mesh, const char* _meshName, int32_t _elementOptions)
	{
		int result = FAIL;

		FbxScene* fbx_scene_p = (FbxScene*)_fbx_scene_p;
		FbxMesh* fbx_mesh_p = nullptr;

		// uncompactified vertex data
		std::vector<SIMPLE_VERTEX> verts_raw;

		// find mesh with specified name, or use first mesh if none is specified
		result = GetFbxMeshFromScene(fbx_mesh_p, fbx_scene_p, _meshName);
		if (FAILED(result))
			return result;

		// extract raw vertices from mesh
		result = ExtractVertsFromFbxMesh(verts_raw, fbx_mesh_p, _elementOptions);
		if (FAILED(result))
			return result;

		// compactify data
		result = CompactifyVerts(verts_raw, _mesh.vertices, _mesh.indices);
		if (FAILED(result))
			return result;
		
		// store vertex and index count
		_mesh.vertex_count = (uint32_t)_mesh.vertices.size();
		_mesh.index_count = (uint32_t)_mesh.indices.size();

		return result;
	}
	int ExtractFbxMaterial(const FbxScene* _fbx_scene_p, SIMPLE_MATERIAL_LIST& _material_list, uint32_t _matNum, int32_t _elementOptions)
	{
		int result = FAIL;

		std::vector<SIMPLE_MATERIAL> materials;
		std::vector<filepath_t> filepaths;

		FbxScene* fbx_scene = (FbxScene*)_fbx_scene_p;
		FbxSurfaceMaterial* fbx_material = nullptr;


		// -- extract material from scene --

		// if material number is specified and scene has a material with that index
		if (_matNum > 0 && fbx_scene->GetGeometryCount() > (int)_matNum)
			fbx_material = fbx_scene->GetMaterial(_matNum);
		// if material number is unspecified or scene does not have a material with that index
		else
			fbx_material = fbx_scene->GetMaterial(0);

		SIMPLE_MATERIAL output_material;

		// skip non-standard materials
		if (fbx_material->Is<FbxSurfaceLambert>() == false)
			return result;

		FbxSurfaceLambert* fbx_lambert = (FbxSurfaceLambert*)fbx_material;

		if (_elementOptions & MATERIAL_ELEMENT::DIFFUSE)
		{
			FbxDouble3 diffuseColor = fbx_lambert->Diffuse.Get();
			FbxDouble diffuseFactor = fbx_lambert->DiffuseFactor.Get();

			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[0] = (float)diffuseColor[0];
			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[1] = (float)diffuseColor[1];
			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[2] = (float)diffuseColor[2];

			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].factor = (float)diffuseFactor;

			FbxFileTexture* fileTexture = fbx_lambert->Diffuse.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_elementOptions & MATERIAL_ELEMENT::EMISSIVE)
		{
			FbxDouble3 emissiveColor = fbx_lambert->Emissive.Get();
			FbxDouble emissiveFactor = fbx_lambert->EmissiveFactor.Get();

			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[0] = (float)emissiveColor[0];
			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[1] = (float)emissiveColor[1];
			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[2] = (float)emissiveColor[2];

			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].factor = (float)emissiveFactor;

			FbxFileTexture* fileTexture = fbx_lambert->Emissive.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_elementOptions & MATERIAL_ELEMENT::SPECULAR)
		{
			if (fbx_material->Is<FbxSurfacePhong>())
			{
				FbxSurfacePhong* phong = (FbxSurfacePhong*)fbx_material;

				FbxDouble3 specularColor = phong->Specular.Get();
				FbxDouble specularFactor = phong->SpecularFactor.Get();

				output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[0] = (float)specularColor[0];
				output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[1] = (float)specularColor[1];
				output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[2] = (float)specularColor[2];

				output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].factor = (float)specularFactor;

				FbxFileTexture* fileTexture = phong->Specular.GetSrcObject<FbxFileTexture>();
				if (fileTexture != nullptr)
				{
					const char* filename = fileTexture->GetRelativeFileName();
					filepath_t filepath;
					strcpy_s(filepath.data(), filepath.max_size(), filename);
					output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].input = filepaths.size();
					filepaths.push_back(filepath);
				}
			}
		}

		if (_elementOptions & MATERIAL_ELEMENT::NORMALMAP)
		{
			FbxDouble3 normalColor = fbx_lambert->NormalMap.Get();

			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[0] = (float)normalColor[0];
			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[1] = (float)normalColor[1];
			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[2] = (float)normalColor[2];

			output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].factor = 1.0f;

			FbxFileTexture* fileTexture = fbx_lambert->NormalMap.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				const char* filename = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), filename);
				output_material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		materials.push_back(output_material);

		// -- /extract material from scene --


		return result;
	}
	int ExtractFbxAnimation(const FbxScene* _fbx_scene_p, SIMPLE_ANIM_CLIP& _anim_clip, uint32_t _elementOptions)
	{
		int result = FAIL;

		FbxScene* fbx_scene_p = (FbxScene*)_fbx_scene_p;
		FbxMesh* fbx_mesh_p = nullptr;


		// get pose count from scene
		uint32_t poseCount = fbx_scene_p->GetPoseCount();
		FbxPose* fbx_bindPose_p = nullptr;


		// -- extract bind pose from scene --

		for (uint32_t i = 0; i < poseCount; i++)
		{
			FbxPose* pose = fbx_scene_p->GetPose(i);
			if (pose->IsBindPose())
			{
				fbx_bindPose_p = pose;
				break;
			}
		}

		// -- /extract bind pose from scene --


		// -- get skeleton root from bind pose --

		uint32_t nodeCount = 0;
		FbxNode* rootNode = nullptr;
		FbxSkeleton* rootSkeleton = nullptr;

		if (fbx_bindPose_p == nullptr)
			return result;
		else
		{
			nodeCount = fbx_bindPose_p->GetCount();
			FbxNode* node = nullptr;
			FbxSkeleton* skeleton = nullptr;

			for (uint32_t i = 0; i < nodeCount; i++)
			{
				node = fbx_bindPose_p->GetNode(i);
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
			FbxNode* fbx_node_p = joints_fbx[i].fbx_node_p;

			for (int32_t c = 0; c < fbx_node_p->GetChildCount(); c++)
			{
				FbxNode* fbx_childNode_p = nullptr;
				FbxSkeleton* fbx_childSkeleton_p = nullptr;

				fbx_childNode_p = fbx_node_p->GetChild(c);

				if (fbx_childNode_p != nullptr)
					fbx_childSkeleton_p = fbx_childNode_p->GetSkeleton();

				if (fbx_childSkeleton_p != nullptr)
				{
					FBX_JOINT childJoint = { fbx_childNode_p, (int)i };
					joints_fbx.push_back(childJoint);
				}
			}
		}

		// -- /create list of joints from skeleton root --


		// -- convert bind pose joint data --

		std::vector<SIMPLE_JOINT> joints_out;

		for (uint32_t i = 0; i < joints_fbx.size(); i++)
		{
			FbxAMatrix transform = joints_fbx[i].fbx_node_p->EvaluateGlobalTransform();

			SIMPLE_JOINT joint = { FbxAMatrixToSimpleMatrix(transform), joints_fbx[i].parent_index };

			joints_out.push_back(joint);
		}

		// -- /convert bind pose joint data --


		// -- get animation data from scene --

		FbxAnimStack* animStack = fbx_scene_p->GetCurrentAnimationStack();
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
					keyframe.transforms.push_back({ FbxAMatrixToSimpleMatrix(joints_fbx[n].fbx_node_p->EvaluateGlobalTransform(keytime)) });

				animClip.frames.push_back(keyframe);
			}
		}

		// -- /get animation data from scene --


		// -- write to file --

		

		// -- /write to file --


		joints_fbx.clear();
		joints_out.clear();


		return result;
	}
#pragma endregion

#pragma region Interface Functions
	int GetScenePolyCount(const char* _fbx_filepath)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbx_filepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

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
	int ExtractMesh(const char* _fbx_filepath, SIMPLE_MESH& _mesh, const char* _meshName, int32_t _meshElements)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbx_filepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// extract mesh from scene
		result = ExtractFbxMesh(scene_p, _mesh, _meshName, _meshElements);

		manager_p->Destroy();

		return result;
	}

	int GetSceneMaterialCount(const char* _fbx_filepath)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbx_filepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// set result default
		result = FAIL;

		// get material count from scene
		result = scene_p->GetMaterialCount();

		manager_p->Destroy();

		return result;
	}
	int ExtractMaterial(const char* _fbx_filepath, SIMPLE_MATERIAL_LIST& _material_list, uint32_t _matNum, int32_t _matElements)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbx_filepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// extract material from scene
		result = ExtractFbxMaterial(scene_p, _material_list, _matNum, _matElements);

		manager_p->Destroy();

		return result;
	}

	int GetScenePoseCount(const char* _fbx_filepath)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbx_filepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// set result default
		result = FAIL;

		// get pose count from scene
		result = scene_p->GetPoseCount();

		manager_p->Destroy();

		return result;
	}
	int ExtractAnimation(const char* _fbx_filepath, SIMPLE_ANIM_CLIP& _anim_clip, uint32_t _animElements)
	{
		int result = FAIL;

		FbxScene* scene_p = nullptr;
		FbxManager* manager_p = nullptr;

		// create FbxManager and import scene from file
		result = CreateAndImport(_fbx_filepath, manager_p, scene_p);
		if (FAILED(result))
			return result;

		// extract animation from scene
		result = ExtractFbxAnimation(scene_p, _anim_clip, _animElements);

		manager_p->Destroy();

		return result;
	}
#pragma endregion

}
