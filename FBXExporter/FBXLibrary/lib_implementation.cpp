#include <array>
#include <fstream>
#include <iostream>

#include "lib_debug.h"
#include "lib_interface.h"
#include "lib_utility.h"


namespace FBXLibrary
{
#pragma region Private Helper Functions
	// FBX helpers

	int GetFbxMeshFromFbxScene(FbxScene* _in_fbx_scene_p, const char* _in_meshName, FbxMesh*& _out_fbx_mesh_p)
	{
		int out_result = FAIL;
		FbxGeometry* fbx_geometry_p = nullptr;

		// search scene for mesh
		for (int i = 0; i < _in_fbx_scene_p->GetGeometryCount(); i++)
		{
			// get next geometry from scene
			fbx_geometry_p = _in_fbx_scene_p->GetGeometry(i);

			// skip non-mesh geometries
			if (fbx_geometry_p->GetAttributeType() == FbxNodeAttribute::eMesh)
				// keep first mesh with a matching name, or keep first mesh in scene if no name is specified
				if (
					(_in_meshName != nullptr && fbx_geometry_p->GetName() == _in_meshName)
					|| _in_meshName == nullptr
					)
				{
					// store geometry as mesh
					_out_fbx_mesh_p = (FbxMesh*)fbx_geometry_p;

					break;
				}
		}

		// verify that a mesh was extracted
		if (_out_fbx_mesh_p != nullptr)
			out_result = SUCCESS;

		return out_result;
	}


	// mesh helpers

	void GetPositionFromFbxControlPoint(const FbxVector4* _in_fbx_controlPoints_p, int _in_controlPointIndex, SIMPLE_VERTEX& _out_vertex)
	{
		// get position from control point (vertex)
		FbxVector4 pos = _in_fbx_controlPoints_p[_in_controlPointIndex];

		// store data
		_out_vertex.pos[0] = (float)pos[0];
		_out_vertex.pos[1] = (float)pos[1];
		_out_vertex.pos[2] = (float)pos[2];
	}
	void GetNormalFromFbxControlPoint(FbxMesh* _in_fbx_mesh_p, int _in_polygonVertexIndex, int _in_polygonVertex, SIMPLE_VERTEX& _out_vertex)
	{
		// get normal element from mesh
		FbxGeometryElementNormal* fbx_normalElement_p = _in_fbx_mesh_p->GetElementNormal();

		// verify normal element exists
		if (fbx_normalElement_p != nullptr)
		{
			// determine normal index
			int normalIndex = _in_polygonVertexIndex;
			if (fbx_normalElement_p->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
				normalIndex = _in_polygonVertex;
			if (fbx_normalElement_p->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
				normalIndex = fbx_normalElement_p->GetIndexArray()[normalIndex];

			// get vertex normal from normal element
			FbxVector4 norm = fbx_normalElement_p->GetDirectArray()[normalIndex];

			// store data
			_out_vertex.norm[0] = (float)norm[0];
			_out_vertex.norm[1] = (float)norm[1];
			_out_vertex.norm[2] = (float)norm[2];
		}
	}
	void GetColorFromFbxControlPoint(FbxMesh* _in_fbx_mesh_p, int _in_polygonVertexIndex, int _in_polygonVertex, SIMPLE_VERTEX& _out_vertex)
	{
		// get color element from mesh
		FbxGeometryElementVertexColor* fbx_colorElement_p = _in_fbx_mesh_p->GetElementVertexColor();

		// verify color element exists
		if (fbx_colorElement_p != nullptr)
		{
			// determine color index
			int colorIndex = _in_polygonVertexIndex;
			if (fbx_colorElement_p->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
				colorIndex = _in_polygonVertex;
			if (fbx_colorElement_p->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
				colorIndex = fbx_colorElement_p->GetIndexArray()[colorIndex];

			// get vertex color from color element
			FbxColor color = fbx_colorElement_p->GetDirectArray()[colorIndex];

			// store data
			_out_vertex.color[0] = (float)color[0];
			_out_vertex.color[1] = (float)color[1];
			_out_vertex.color[2] = (float)color[2];
			_out_vertex.color[3] = (float)color[3];
		}
	}
	void GetTexCoordFromFbxControlPoint(FbxMesh* _in_fbx_mesh_p, int _in_polygonVertexIndex, int _in_polygonVertex, SIMPLE_VERTEX& _out_vertex)
	{
		// get UV element from mesh
		FbxGeometryElementUV* fbx_uvElement_p = _in_fbx_mesh_p->GetElementUV();

		// verify UV element exists
		if (fbx_uvElement_p != nullptr)
		{
			// determine texture coordinate index
			int texCoordIndex = _in_polygonVertexIndex;
			if (fbx_uvElement_p->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
				texCoordIndex = _in_polygonVertex;
			if (fbx_uvElement_p->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
				texCoordIndex = fbx_uvElement_p->GetIndexArray()[texCoordIndex];

			// get vertex texcoord from UV element
			FbxVector2 texCoord = fbx_uvElement_p->GetDirectArray()[texCoordIndex];

			// store data
			_out_vertex.texCoord[0] = (float)texCoord[0];
			_out_vertex.texCoord[1] = (float)(1.0f - texCoord[1]);
		}
	}

	void GetElementsFromFbxControlPoint(const FbxVector4* _in_fbx_controlPoints_p, const FbxMesh* _in_fbx_mesh_p,
		int _in_polygonVertexIndex, int _in_polygonVertex, uint32_t _in_elementsToExtract, SIMPLE_VERTEX& _out_vertex)
	{
		// get elements from FBX control point if flags are set
		if (_in_elementsToExtract & MESH_ELEMENT::POSITION)
			GetPositionFromFbxControlPoint(_in_fbx_controlPoints_p, _in_polygonVertex, _out_vertex);
		if (_in_elementsToExtract & MESH_ELEMENT::NORMAL)
			GetNormalFromFbxControlPoint((FbxMesh*)_in_fbx_mesh_p, _in_polygonVertexIndex, _in_polygonVertex, _out_vertex);
		if (_in_elementsToExtract & MESH_ELEMENT::COLOR)
			GetColorFromFbxControlPoint((FbxMesh*)_in_fbx_mesh_p, _in_polygonVertexIndex, _in_polygonVertex, _out_vertex);
		if (_in_elementsToExtract & MESH_ELEMENT::TEXCOORD)
			GetTexCoordFromFbxControlPoint((FbxMesh*)_in_fbx_mesh_p, _in_polygonVertexIndex, _in_polygonVertex, _out_vertex);
	}

	int GetVerticesFromFbxMesh(FbxMesh* _in_fbx_mesh_p, uint32_t _in_elementsToExtract, std::vector<SIMPLE_VERTEX>& _out_vertices)
	{
		int out_result = FAIL;

		// verify that fbx is initialized
		if (_in_fbx_mesh_p != nullptr)
		{
			// number of polygons in mesh
			int polygonCount = _in_fbx_mesh_p->GetPolygonCount();

			// array of FBX polygon vertices (equivalent to vertex indices)
			int* polygonVertices_p = _in_fbx_mesh_p->GetPolygonVertices();

			// array of FBX control points (equivalent to vertices)
			const FbxVector4* fbx_controlPoints_p = _in_fbx_mesh_p->GetControlPoints();


			// for each polygon in mesh
			for (int i = 0; i < polygonCount; i++)
			{
				// for each vertex in polygon
				for (int v = 0; v < 3; v++)
				{
					// vertex to read data into
					SIMPLE_VERTEX vertex;

					// position of vertex's index in index list
					int polygonVertexIndex = i * 3 + v;

					// vertex's index from index list
					int polygonVertex = polygonVertices_p[polygonVertexIndex];

					// get elements for current vertex
					GetElementsFromFbxControlPoint(fbx_controlPoints_p, _in_fbx_mesh_p, polygonVertexIndex, polygonVertex, _in_elementsToExtract, vertex);

					// add vertex to list
					_out_vertices.push_back(vertex);
				}
			}

			// verify vertices were extracted from mesh
			if (_out_vertices.size() > 0)
				out_result = SUCCESS;
		}

		return out_result;
	}

	int CompactifyVertices(std::vector<SIMPLE_VERTEX>& _in_vertices, std::vector<SIMPLE_VERTEX>& _out_vertices, std::vector<uint32_t> _out_indices)
	{
		int out_result = FAIL;

		// for all raw vertices
		for (int i = 0; i < _in_vertices.size(); i++)
		{
			// get current vertex
			SIMPLE_VERTEX vertex = _in_vertices[i];

			// index to add to index list
			uint32_t index = 0;

			bool vertexIsUnique = true;

			// for all unique vertices
			for (int v = 0; v < _out_vertices.size(); v++)
			{
				// if current vertex matches a unique vertex
				if (vertex == _out_vertices[v])
				{
					// mark current vertex as not unique
					vertexIsUnique = false;

					// store index of unique vertex that the current vertex is equivalent to
					index = v;

					break;
				}
			}


			if (vertexIsUnique)
			{
				// new index is the number of existing unique vertices
				index = (uint32_t)_out_vertices.size();

				// add current vertex to unique vertex list
				_out_vertices.push_back(vertex);
			}

			// add index to index list
			_out_indices.push_back(index);
		}

		// verify vertices and indices were generated
		if (_out_vertices.size() > 0 && _out_indices.size() > 0)
			out_result = SUCCESS;

		return out_result;
	}


	// material helpers


	// animation helpers


#pragma endregion

#pragma region Utility Functions
	// FBX functions

	int CreateFbxManagerAndImportFbxScene(const char* _in_fbx_filepath, FbxManager*& _out_fbx_manager_p, FbxScene*& _out_fbx_scene_p)
	{
		// return INVALID_ARG if scene is not nullptr
		if (_out_fbx_scene_p != nullptr)
			return INVALID_ARG;

		// sdk manager to handle memory management
		_out_fbx_manager_p = FbxManager::Create();

		// return FAIL if manager was not created
		if (_out_fbx_manager_p == nullptr)
			return FAIL;

		// create IO settings object
		FbxIOSettings* fbx_iosettings_p = FbxIOSettings::Create(_out_fbx_manager_p, "");
		_out_fbx_manager_p->SetIOSettings(fbx_iosettings_p);

		// create importer using sdk manager
		FbxImporter* fbx_importer_p = FbxImporter::Create(_out_fbx_manager_p, "");

		// initialize importer, or display error info and return FAIL if initialization failed
		if (!fbx_importer_p->Initialize(_in_fbx_filepath, -1, _out_fbx_manager_p->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", fbx_importer_p->GetStatus().GetErrorString());
			return FAIL;
		}

		// create new scene to populate with imported file
		_out_fbx_scene_p = FbxScene::Create(_out_fbx_manager_p, "imported_scene");

		// import contents of file into scene
		fbx_importer_p->Import(_out_fbx_scene_p);

		// importer is no longer needed
		fbx_importer_p->Destroy();

		// return FAIL if scene was not imported
		if (_out_fbx_scene_p == nullptr)
			return FAIL;

		return SUCCESS;
	}


	// conversion functions

	SIMPLE_MATRIX ConvertFbxAMatrixToSimpleMatrix(FbxAMatrix _in_fbx_matrix)
	{
		return
		{
			(float)_in_fbx_matrix.Get(0, 0),
			(float)_in_fbx_matrix.Get(0, 1),
			(float)_in_fbx_matrix.Get(0, 2),
			(float)_in_fbx_matrix.Get(0, 3),

			(float)_in_fbx_matrix.Get(1, 0),
			(float)_in_fbx_matrix.Get(1, 1),
			(float)_in_fbx_matrix.Get(1, 2),
			(float)_in_fbx_matrix.Get(1, 3),

			(float)_in_fbx_matrix.Get(2, 0),
			(float)_in_fbx_matrix.Get(2, 1),
			(float)_in_fbx_matrix.Get(2, 2),
			(float)_in_fbx_matrix.Get(2, 3),

			(float)_in_fbx_matrix.Get(3, 0),
			(float)_in_fbx_matrix.Get(3, 1),
			(float)_in_fbx_matrix.Get(3, 2),
			(float)_in_fbx_matrix.Get(3, 3)
		};
	}


	// data extraction functions

	int GetMeshFromFbxScene(const FbxScene* _in_fbx_scene_p, const char* _in_meshName, int32_t _in_elementsToExtract, SIMPLE_MESH& _out_mesh)
	{
		int out_result = FAIL;

		FbxScene* fbx_scene_p = (FbxScene*)_in_fbx_scene_p;
		FbxMesh* fbx_mesh_p = nullptr;

		// uncompactified vertex data
		std::vector<SIMPLE_VERTEX> vertices_raw;

		// find mesh with specified name, or use first mesh if none is specified
		out_result = GetFbxMeshFromFbxScene(fbx_scene_p, _in_meshName, fbx_mesh_p);
		if (FAILED(out_result))
			return out_result;

		// extract raw vertices from mesh
		out_result = GetVerticesFromFbxMesh(fbx_mesh_p, _in_elementsToExtract, vertices_raw);
		if (FAILED(out_result))
			return out_result;

		// compactify data
		out_result = CompactifyVertices(vertices_raw, _out_mesh.vertices, _out_mesh.indices);
		if (FAILED(out_result))
			return out_result;

		// store vertex and index count
		_out_mesh.vertex_count = (uint32_t)_out_mesh.vertices.size();
		_out_mesh.index_count = (uint32_t)_out_mesh.indices.size();

		return out_result;
	}
	int GetMaterialsFromFbxScene(const FbxScene* _in_fbx_scene_p, uint32_t _in_materialNum, int32_t _in_elementsToExtract, SIMPLE_MATERIAL_LIST& _out_material_list)
	{
		int out_result = FAIL;

		FbxScene* fbx_scene_p = (FbxScene*)_in_fbx_scene_p;
		FbxSurfaceMaterial* fbx_material_p = nullptr;

		// list of material data containers
		std::vector<SIMPLE_MATERIAL> materials;

		// list of texture filepaths
		std::vector<filepath_t> filepaths;


		// -- extract material from scene --

		// if material number is specified and scene has a material with that index
		if (_in_materialNum > 0 && fbx_scene_p->GetGeometryCount() > (int)_in_materialNum)
			fbx_material_p = fbx_scene_p->GetMaterial(_in_materialNum);
		// if material number is unspecified or scene does not have a material with that index
		else
			fbx_material_p = fbx_scene_p->GetMaterial(0);

		// material to copy data into from FBX material
		SIMPLE_MATERIAL material;

		// skip non-standard materials
		if (fbx_material_p->Is<FbxSurfaceLambert>() == false)
			return out_result;

		FbxSurfaceLambert* fbx_lambert_p = (FbxSurfaceLambert*)fbx_material_p;
		FbxFileTexture* fbx_fileTexture_p = nullptr;
		const char* textureFilepath = "";

		if (_in_elementsToExtract & MATERIAL_ELEMENT::DIFFUSE)
		{
			FbxDouble3 diffuseColor = fbx_lambert_p->Diffuse.Get();
			FbxDouble diffuseFactor = fbx_lambert_p->DiffuseFactor.Get();

			material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[0] = (float)diffuseColor[0];
			material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[1] = (float)diffuseColor[1];
			material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].value[2] = (float)diffuseColor[2];

			material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].factor = (float)diffuseFactor;

			fbx_fileTexture_p = fbx_lambert_p->Diffuse.GetSrcObject<FbxFileTexture>();

			if (fbx_fileTexture_p != nullptr)
			{
				textureFilepath = fbx_fileTexture_p->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

				material[SIMPLE_MATERIAL::COMPONENT_TYPE::DIFFUSE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_in_elementsToExtract & MATERIAL_ELEMENT::EMISSIVE)
		{
			FbxDouble3 emissiveColor = fbx_lambert_p->Emissive.Get();
			FbxDouble emissiveFactor = fbx_lambert_p->EmissiveFactor.Get();

			material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[0] = (float)emissiveColor[0];
			material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[1] = (float)emissiveColor[1];
			material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].value[2] = (float)emissiveColor[2];

			material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].factor = (float)emissiveFactor;

			fbx_fileTexture_p = fbx_lambert_p->Emissive.GetSrcObject<FbxFileTexture>();

			if (fbx_fileTexture_p != nullptr)
			{
				textureFilepath = fbx_fileTexture_p->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

				material[SIMPLE_MATERIAL::COMPONENT_TYPE::EMISSIVE].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		if (_in_elementsToExtract & MATERIAL_ELEMENT::SPECULAR)
		{
			if (fbx_material_p->Is<FbxSurfacePhong>())
			{
				FbxSurfacePhong* fbx_phong_p = (FbxSurfacePhong*)fbx_material_p;

				FbxDouble3 specularColor = fbx_phong_p->Specular.Get();
				FbxDouble specularFactor = fbx_phong_p->SpecularFactor.Get();

				material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[0] = (float)specularColor[0];
				material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[1] = (float)specularColor[1];
				material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].value[2] = (float)specularColor[2];

				material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].factor = (float)specularFactor;

				fbx_fileTexture_p = fbx_phong_p->Specular.GetSrcObject<FbxFileTexture>();

				if (fbx_fileTexture_p != nullptr)
				{
					textureFilepath = fbx_fileTexture_p->GetRelativeFileName();
					filepath_t filepath;
					strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

					material[SIMPLE_MATERIAL::COMPONENT_TYPE::SPECULAR].input = filepaths.size();
					filepaths.push_back(filepath);
				}
			}
		}

		if (_in_elementsToExtract & MATERIAL_ELEMENT::NORMALMAP)
		{
			FbxDouble3 normalColor = fbx_lambert_p->NormalMap.Get();

			material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[0] = (float)normalColor[0];
			material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[1] = (float)normalColor[1];
			material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].value[2] = (float)normalColor[2];

			material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].factor = 1.0f;

			FbxFileTexture* fileTexture = fbx_lambert_p->NormalMap.GetSrcObject<FbxFileTexture>();
			if (fileTexture != nullptr)
			{
				textureFilepath = fileTexture->GetRelativeFileName();
				filepath_t filepath;
				strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

				material[SIMPLE_MATERIAL::COMPONENT_TYPE::NORMALMAP].input = filepaths.size();
				filepaths.push_back(filepath);
			}
		}

		materials.push_back(material);

		// -- /extract material from scene --


		return out_result;
	}
	int GetAnimationFromFbxScene(const FbxScene* _in_fbx_scene_p, uint32_t _in_elementsToExtract, SIMPLE_ANIM_CLIP& _out_animClip)
	{
		int result = FAIL;

		FbxScene* fbx_scene_p = (FbxScene*)_in_fbx_scene_p;
		FbxMesh* fbx_mesh_p = nullptr;
		FbxPose* fbx_bindPose_p = nullptr;


		// get pose count from scene
		uint32_t poseCount = fbx_scene_p->GetPoseCount();

		// -- extract bind pose from scene --

		for (uint32_t i = 0; i < poseCount; i++)
		{
			FbxPose* fbx_pose_p = fbx_scene_p->GetPose(i);
			if (fbx_pose_p->IsBindPose())
			{
				fbx_bindPose_p = fbx_pose_p;
				break;
			}
		}

		// -- /extract bind pose from scene --


		// -- get skeleton root from bind pose --

		uint32_t nodeCount = 0;
		FbxNode* fbx_rootNode_p = nullptr;
		FbxSkeleton* fbx_rootSkeleton_p = nullptr;

		if (fbx_bindPose_p == nullptr)
			return result;
		else
		{
			nodeCount = fbx_bindPose_p->GetCount();
			FbxNode* fbx_node_p = nullptr;
			FbxSkeleton* fbx_skeleton_p = nullptr;

			for (uint32_t i = 0; i < nodeCount; i++)
			{
				fbx_node_p = fbx_bindPose_p->GetNode(i);
				if (fbx_node_p != nullptr)
				{
					fbx_skeleton_p = fbx_node_p->GetSkeleton();
					if (fbx_skeleton_p != nullptr && fbx_skeleton_p->IsSkeletonRoot())
					{
						fbx_rootNode_p = fbx_node_p;
						fbx_rootSkeleton_p = fbx_skeleton_p;
						break;
					}
				}
			}
		}

		// -- /get skeleton root from bind pose --


		// -- create list of joints from skeleton root --

		std::vector<FBX_JOINT> joints_fbx;

		FBX_JOINT rootJoint = { fbx_rootNode_p, -1 };
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

			SIMPLE_JOINT joint = { ConvertFbxAMatrixToSimpleMatrix(transform), joints_fbx[i].parent_index };

			joints_out.push_back(joint);
		}

		// -- /convert bind pose joint data --


		// -- get animation data from scene --

		FbxAnimStack* fbx_animStack_p = fbx_scene_p->GetCurrentAnimationStack();

		if (fbx_animStack_p == nullptr)
			return result;
		else
		{
			FbxTime::EMode mode = FbxTime::EMode::eFrames30;

			// get animation duration and frame count
			FbxTime animDuration = fbx_animStack_p->GetLocalTimeSpan().GetDuration();
			int64_t frameCount = animDuration.GetFrameCount(mode);

			// store duration in seconds
			_out_animClip.duration = animDuration.GetSecondDouble();

			for (int64_t i = 1; i < frameCount; i++) // starts at 1 to skip bind post at frame 0
			{
				SIMPLE_ANIM_FRAME frame;

				// get keytime for current frame
				FbxTime frameTime;
				frameTime.SetFrame(i, mode);

				// store keytime in seconds
				frame.time = frameTime.GetSecondDouble();

				// get node transforms for current frame
				for (uint32_t n = 0; n < joints_fbx.size(); n++)
					frame.transforms.push_back({ ConvertFbxAMatrixToSimpleMatrix(joints_fbx[n].fbx_node_p->EvaluateGlobalTransform(frameTime)) });

				_out_animClip.frames.push_back(frame);
			}
		}

		// -- /get animation data from scene --

		return result;
	}
#pragma endregion

#pragma region Interface Functions
	int GetMeshFromFbxFile(const char* _in_fbx_filepath, const char* _in_meshName, int32_t _in_elementsToExtract, SIMPLE_MESH& _out_mesh)
	{
		int out_result = FAIL;

		FbxScene* fbx_scene_p = nullptr;
		FbxManager* fbx_manager_p = nullptr;

		// create manager and import scene from file
		out_result = CreateFbxManagerAndImportFbxScene(_in_fbx_filepath, fbx_manager_p, fbx_scene_p);
		if (FAILED(out_result))
			return out_result;

		// extract data from scene into storage mesh
		out_result = GetMeshFromFbxScene(fbx_scene_p, _out_mesh, _in_meshName, _in_elementsToExtract);

		// manager is no longer needed
		fbx_manager_p->Destroy();

		return out_result;
	}
	int GetMaterialsFromFbxScene(const char* _in_fbx_filepath, uint32_t _in_materialNum, int32_t _in_elementsToExtract, SIMPLE_MATERIAL_LIST& _out_materialList)
	{
		int out_result = FAIL;

		FbxScene* fbx_scene_p = nullptr;
		FbxManager* fbx_manager_p = nullptr;

		// create manager and import scene from file
		out_result = CreateFbxManagerAndImportFbxScene(_in_fbx_filepath, fbx_manager_p, fbx_scene_p);
		if (FAILED(out_result))
			return out_result;

		// extract data from scene into material list
		out_result = GetMaterialsFromFbxScene(fbx_scene_p, _out_materialList, _in_materialNum, _in_elementsToExtract);

		fbx_manager_p->Destroy();

		return out_result;
	}
	int GetAnimationFromFbxScene(const char* _in_fbx_filepath, uint32_t _in_elementsToExtract, SIMPLE_ANIM_CLIP& _out_animClip)
	{
		int out_result = FAIL;

		FbxScene* fbx_scene_p = nullptr;
		FbxManager* fbx_manager_p = nullptr;

		// create manager and import scene from file
		out_result = CreateFbxManagerAndImportFbxScene(_in_fbx_filepath, fbx_manager_p, fbx_scene_p);
		if (FAILED(out_result))
			return out_result;

		// extract animation from scene
		out_result = GetAnimationFromFbxScene(fbx_scene_p, _in_elementsToExtract, _out_animClip);

		// manager is no longer needed
		fbx_manager_p->Destroy();

		return out_result;
	}
#pragma endregion

}
