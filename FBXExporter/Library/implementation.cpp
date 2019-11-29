#include "interface.h"
#include "utility.h"

#include <cstring>
#include <iostream>

#include "debug.h"


namespace fbx_exporter
{
	namespace library
	{
#pragma region Private Helper Functions
		Result GetFbxMeshFromFbxScene(
			const FbxScene*				_in_fbxScene_p
			, const char*				_in_meshName
			, FbxMesh*&					_out_fbxMesh_p
		) {
			Result ret_result = Result::FAIL;
			FbxGeometry* fbx_geometry_p = nullptr;

			// search scene for mesh
			for (int i = 0; i < _in_fbxScene_p->GetGeometryCount(); i++)
			{
				fbx_geometry_p = ((FbxScene*)_in_fbxScene_p)->GetGeometry(i);

				// skip non-mesh geometries
				if (fbx_geometry_p->GetAttributeType() == FbxNodeAttribute::eMesh)
					// keep first mesh with a matching name,
					// or keep first mesh in scene if no name is specified
					if ((_in_meshName != nullptr && fbx_geometry_p->GetName() == _in_meshName)
						|| _in_meshName == nullptr)
					{
						_out_fbxMesh_p = (FbxMesh*)fbx_geometry_p;
						break;
					}
			}

			// verify that a mesh was extracted
			if (_out_fbxMesh_p != nullptr)
				ret_result = Result::SUCCESS;

			return ret_result;
		}

		void GetPositionFromFbxControlPoint(
			const FbxVector4*			_in_fbxControlPoints_p
			, const int					_in_controlPointIndex
			, Vertex&					_out_vertex
		) {
			FbxVector4 pos = _in_fbxControlPoints_p[_in_controlPointIndex];
			_out_vertex.pos[0] = (float)pos[0];
			_out_vertex.pos[1] = (float)pos[1];
			_out_vertex.pos[2] = (float)pos[2];
		}
		void GetNormalFromFbxControlPoint(
			const FbxMesh*				_in_fbxMesh_p
			, const int					_in_polygonVertexIndex
			, const int					_in_polygonVertex
			, Vertex&					_out_vertex
		) {
			FbxGeometryElementNormal* fbx_normalElement_p =
				((FbxMesh*)_in_fbxMesh_p)->GetElementNormal();

			// verify normal element exists
			if (fbx_normalElement_p != nullptr)
			{
				// determine normal index
				int normalIndex = _in_polygonVertexIndex;
				if (fbx_normalElement_p->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
					normalIndex = _in_polygonVertex;
				if (fbx_normalElement_p->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
					normalIndex = fbx_normalElement_p->GetIndexArray()[normalIndex];

				FbxVector4 norm = fbx_normalElement_p->GetDirectArray()[normalIndex];
				_out_vertex.norm[0] = (float)norm[0];
				_out_vertex.norm[1] = (float)norm[1];
				_out_vertex.norm[2] = (float)norm[2];
			}
		}
		void GetColorFromFbxControlPoint(
			const FbxMesh*				_in_fbxMesh_p
			, const int					_in_polygonVertexIndex
			, const int					_in_polygonVertex
			, Vertex&					_out_vertex
		) {
			FbxGeometryElementVertexColor* fbx_colorElement_p =
				((FbxMesh*)_in_fbxMesh_p)->GetElementVertexColor();

			// verify color element exists
			if (fbx_colorElement_p != nullptr)
			{
				// determine color index
				int colorIndex = _in_polygonVertexIndex;
				if (fbx_colorElement_p->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
					colorIndex = _in_polygonVertex;
				if (fbx_colorElement_p->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
					colorIndex = fbx_colorElement_p->GetIndexArray()[colorIndex];

				FbxColor color = fbx_colorElement_p->GetDirectArray()[colorIndex];
				_out_vertex.color[0] = (float)color[0];
				_out_vertex.color[1] = (float)color[1];
				_out_vertex.color[2] = (float)color[2];
				_out_vertex.color[3] = (float)color[3];
			}
		}
		void GetTexCoordFromFbxControlPoint(
			const FbxMesh*				_in_fbxMesh_p
			, const int					_in_polygonVertexIndex
			, const int					_in_polygonVertex
			, Vertex&					_out_vertex
		) {
			FbxGeometryElementUV* fbx_uvElement_p = ((FbxMesh*)_in_fbxMesh_p)->GetElementUV();

			// verify UV element exists
			if (fbx_uvElement_p != nullptr)
			{
				// determine texture coordinate index
				int texCoordIndex = _in_polygonVertexIndex;
				if (fbx_uvElement_p->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
					texCoordIndex = _in_polygonVertex;
				if (fbx_uvElement_p->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
					texCoordIndex = fbx_uvElement_p->GetIndexArray()[texCoordIndex];

				FbxVector2 texCoord = fbx_uvElement_p->GetDirectArray()[texCoordIndex];
				_out_vertex.texCoord[0] = (float)texCoord[0];
				_out_vertex.texCoord[1] = (float)(1.0f - texCoord[1]);
			}
		}

		void GetElementsFromFbxControlPoint(
			const FbxVector4*			_in_fbxControlPoints_p
			, const FbxMesh*			_in_fbxMesh_p
			, const int					_in_polygonVertexIndex
			, const int					_in_polygonVertex
			, const uint32_t			_in_elementsToExtract
			, Vertex&					_out_vertex
		) {
			if (_in_elementsToExtract & static_cast<int>(MeshElement::POSITION))
				GetPositionFromFbxControlPoint(_in_fbxControlPoints_p, _in_polygonVertex,
					_out_vertex);

			if (_in_elementsToExtract & static_cast<int>(MeshElement::NORMAL))
				GetNormalFromFbxControlPoint((FbxMesh*)_in_fbxMesh_p, _in_polygonVertexIndex,
					_in_polygonVertex, _out_vertex);

			if (_in_elementsToExtract & static_cast<int>(MeshElement::COLOR))
				GetColorFromFbxControlPoint((FbxMesh*)_in_fbxMesh_p, _in_polygonVertexIndex,
					_in_polygonVertex, _out_vertex);

			if (_in_elementsToExtract & static_cast<int>(MeshElement::TEXCOORD))
				GetTexCoordFromFbxControlPoint((FbxMesh*)_in_fbxMesh_p, _in_polygonVertexIndex,
					_in_polygonVertex, _out_vertex);
		}

		Result GetVerticesFromFbxMesh(
			const FbxMesh*				_in_fbxMesh_p
			, const uint32_t			_in_elementsToExtract
			, std::vector<Vertex>&		_out_vertices
		) {
			Result ret_result = Result::FAIL;

			// verify that fbx mesh is initialized
			if (_in_fbxMesh_p != nullptr)
			{
				int polygonCount = _in_fbxMesh_p->GetPolygonCount();

				// array of FBX polygon vertices (equivalent to vertex indices)
				int* polygonVertices_p = _in_fbxMesh_p->GetPolygonVertices();

				// array of FBX control points (equivalent to vertices)
				const FbxVector4* fbx_controlPoints_p = _in_fbxMesh_p->GetControlPoints();


				// for each polygon in mesh
				for (int i = 0; i < polygonCount; i++)
				{
					// for each vertex in polygon
					for (int v = 0; v < 3; v++)
					{
						Vertex vertex;

						// position of vertex's index in index list
						int polygonVertexIndex = i * 3 + v;

						// vertex's index from index list
						int polygonVertex = polygonVertices_p[polygonVertexIndex];

						GetElementsFromFbxControlPoint(fbx_controlPoints_p, _in_fbxMesh_p, polygonVertexIndex, polygonVertex, _in_elementsToExtract, vertex);
						_out_vertices.push_back(vertex);
					}
				}

				// verify vertices were extracted from mesh
				if (_out_vertices.size() > 0)
					ret_result = Result::SUCCESS;
			}

			return ret_result;
		}

		Result CompactifyVertices(
			const std::vector<Vertex>&	_in_vertices
			, std::vector<Vertex>&		_out_vertices
			, std::vector<uint32_t>&	_out_indices
		) {
			Result ret_result = Result::FAIL;

			// for all raw vertices
			for (uint32_t i = 0; i < _in_vertices.size(); i++)
			{
				Vertex vertex = _in_vertices[i];
				uint32_t index = 0;

				bool vertexIsUnique = true;

				// compare current vertex against each unique vertex
				for (uint32_t v = 0; v < _out_vertices.size(); v++)
				{
					if (vertex == _out_vertices[v])
					{
						vertexIsUnique = false;
						index = v;
						break;
					}
				}

				if (vertexIsUnique)
				{
					index = (uint32_t)_out_vertices.size();
					_out_vertices.push_back(vertex);
				}

				_out_indices.push_back(index);
			}

			// verify vertices and indices were generated
			if (_out_vertices.size() > 0 && _out_indices.size() > 0)
				ret_result = Result::SUCCESS;

			return ret_result;
		}
#pragma endregion

#pragma region Utility Function Definitions
		Result CreateFbxManagerAndImportFbxScene(
			const char*					_in_fbxFilepath
			, FbxManager*&				_out_fbxManager_p
			, FbxScene*&				_out_fbxScene_p
		) {
			// ensure scene is uninitialized
			if (_out_fbxScene_p != nullptr)
				return Result::INVALID_ARG;

			_out_fbxManager_p = FbxManager::Create();

			// ensure manager was created
			if (_out_fbxManager_p == nullptr)
				return Result::FAIL;

			FbxIOSettings* fbxIOSettings_p = FbxIOSettings::Create(_out_fbxManager_p, "");
			_out_fbxManager_p->SetIOSettings(fbxIOSettings_p);

			FbxImporter* fbxImporter_p = FbxImporter::Create(_out_fbxManager_p, "");

			// initialize importer, or display error info and return FAIL if initialization failed
			if (!fbxImporter_p->Initialize(_in_fbxFilepath, -1, _out_fbxManager_p->GetIOSettings())) {
				std::cout << ("Call to FbxImporter::Initialize() failed.\n");
				std::cout << ("Error returned: %s\n\n", fbxImporter_p->GetStatus().GetErrorString());
				return Result::FAIL;
			}

			_out_fbxScene_p = FbxScene::Create(_out_fbxManager_p, "imported_scene");
			fbxImporter_p->Import(_out_fbxScene_p);
			fbxImporter_p->Destroy();

			// ensure scene was imported
			if (_out_fbxScene_p == nullptr)
				return Result::FAIL;

			return Result::SUCCESS;
		}

		Matrix ConvertFbxAMatrixToMatrix(FbxAMatrix _in_fbxMatrix)
		{
			return
			{
				(float)_in_fbxMatrix.Get(0, 0),
				(float)_in_fbxMatrix.Get(0, 1),
				(float)_in_fbxMatrix.Get(0, 2),
				(float)_in_fbxMatrix.Get(0, 3),

				(float)_in_fbxMatrix.Get(1, 0),
				(float)_in_fbxMatrix.Get(1, 1),
				(float)_in_fbxMatrix.Get(1, 2),
				(float)_in_fbxMatrix.Get(1, 3),

				(float)_in_fbxMatrix.Get(2, 0),
				(float)_in_fbxMatrix.Get(2, 1),
				(float)_in_fbxMatrix.Get(2, 2),
				(float)_in_fbxMatrix.Get(2, 3),

				(float)_in_fbxMatrix.Get(3, 0),
				(float)_in_fbxMatrix.Get(3, 1),
				(float)_in_fbxMatrix.Get(3, 2),
				(float)_in_fbxMatrix.Get(3, 3)
			};
		}

		Result GetMeshFromFbxScene(
			const FbxScene*				_in_fbxScene_p
			, const char*				_in_meshName
			, const uint32_t			_in_elementsToExtract
			, Mesh&						_out_mesh
		) {
			Result ret_result = Result::FAIL;

			FbxScene* fbxScene_p = (FbxScene*)_in_fbxScene_p;
			FbxMesh* fbxMesh_p = nullptr;

			std::vector<Vertex> rawVertices;

			ret_result = GetFbxMeshFromFbxScene(fbxScene_p, _in_meshName, fbxMesh_p);
			if (!Succeeded(ret_result))
				return ret_result;

			ret_result = GetVerticesFromFbxMesh(fbxMesh_p, _in_elementsToExtract, rawVertices);
			if (!Succeeded(ret_result))
				return ret_result;

			ret_result = CompactifyVertices(rawVertices, _out_mesh.vertices, _out_mesh.indices);
			if (!Succeeded(ret_result))
				return ret_result;

			_out_mesh.vertex_count = (uint32_t)_out_mesh.vertices.size();
			_out_mesh.index_count = (uint32_t)_out_mesh.indices.size();

			return ret_result;
		}
		Result GetMaterialsFromFbxScene(
			const FbxScene*				_in_fbxScene_p
			, const uint32_t			_in_materialNum
			, const uint32_t			_in_elementsToExtract
			, MaterialList&				_out_materialList
		) {
			Result ret_result = Result::FAIL;

			FbxScene* fbxScene_p = (FbxScene*)_in_fbxScene_p;
			FbxSurfaceMaterial* fbxMaterial_p = nullptr;
			std::vector<Material> materials;
			std::vector<filepath_t> filepaths;

			// TODO: split function into subfunctions (see GetMeshFromFbxScene)


			// -- extract material from scene --

			// if material number is specified and scene has a material with that index
			if (_in_materialNum > 0 && fbxScene_p->GetGeometryCount() > (int)_in_materialNum)
				fbxMaterial_p = fbxScene_p->GetMaterial(_in_materialNum);
			// if material number is unspecified or scene does not have a material with that index
			else
				fbxMaterial_p = fbxScene_p->GetMaterial(0);

			// material to copy data into from FBX material
			Material material;

			// skip non-standard materials
			if (fbxMaterial_p->Is<FbxSurfaceLambert>() == false)
				return ret_result;

			FbxSurfaceLambert* fbx_lambert_p = (FbxSurfaceLambert*)fbxMaterial_p;
			FbxFileTexture* fbx_fileTexture_p = nullptr;
			const char* textureFilepath = "";

			if (_in_elementsToExtract & static_cast<int>(MaterialElement::DIFFUSE))
			{
				FbxDouble3 diffuseColor = fbx_lambert_p->Diffuse.Get();
				FbxDouble diffuseFactor = fbx_lambert_p->DiffuseFactor.Get();

				material[Material::ComponentType::DIFFUSE].value[0] = (float)diffuseColor[0];
				material[Material::ComponentType::DIFFUSE].value[1] = (float)diffuseColor[1];
				material[Material::ComponentType::DIFFUSE].value[2] = (float)diffuseColor[2];

				material[Material::ComponentType::DIFFUSE].factor = (float)diffuseFactor;

				fbx_fileTexture_p = fbx_lambert_p->Diffuse.GetSrcObject<FbxFileTexture>();

				if (fbx_fileTexture_p != nullptr)
				{
					textureFilepath = fbx_fileTexture_p->GetRelativeFileName();
					filepath_t filepath;
					strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

					material[Material::ComponentType::DIFFUSE].input = filepaths.size();
					filepaths.push_back(filepath);
				}
			}

			if (_in_elementsToExtract & static_cast<int>(MaterialElement::EMISSIVE))
			{
				FbxDouble3 emissiveColor = fbx_lambert_p->Emissive.Get();
				FbxDouble emissiveFactor = fbx_lambert_p->EmissiveFactor.Get();

				material[Material::ComponentType::EMISSIVE].value[0] = (float)emissiveColor[0];
				material[Material::ComponentType::EMISSIVE].value[1] = (float)emissiveColor[1];
				material[Material::ComponentType::EMISSIVE].value[2] = (float)emissiveColor[2];

				material[Material::ComponentType::EMISSIVE].factor = (float)emissiveFactor;

				fbx_fileTexture_p = fbx_lambert_p->Emissive.GetSrcObject<FbxFileTexture>();

				if (fbx_fileTexture_p != nullptr)
				{
					textureFilepath = fbx_fileTexture_p->GetRelativeFileName();
					filepath_t filepath;
					strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

					material[Material::ComponentType::EMISSIVE].input = filepaths.size();
					filepaths.push_back(filepath);
				}
			}

			if (_in_elementsToExtract & static_cast<int>(MaterialElement::SPECULAR))
			{
				if (fbxMaterial_p->Is<FbxSurfacePhong>())
				{
					FbxSurfacePhong* fbx_phong_p = (FbxSurfacePhong*)fbxMaterial_p;

					FbxDouble3 specularColor = fbx_phong_p->Specular.Get();
					FbxDouble specularFactor = fbx_phong_p->SpecularFactor.Get();

					material[Material::ComponentType::SPECULAR].value[0] = (float)specularColor[0];
					material[Material::ComponentType::SPECULAR].value[1] = (float)specularColor[1];
					material[Material::ComponentType::SPECULAR].value[2] = (float)specularColor[2];

					material[Material::ComponentType::SPECULAR].factor = (float)specularFactor;

					fbx_fileTexture_p = fbx_phong_p->Specular.GetSrcObject<FbxFileTexture>();

					if (fbx_fileTexture_p != nullptr)
					{
						textureFilepath = fbx_fileTexture_p->GetRelativeFileName();
						filepath_t filepath;
						strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

						material[Material::ComponentType::SPECULAR].input = filepaths.size();
						filepaths.push_back(filepath);
					}
				}
			}

			if (_in_elementsToExtract & static_cast<int>(MaterialElement::NORMALMAP))
			{
				FbxDouble3 normalColor = fbx_lambert_p->NormalMap.Get();

				material[Material::ComponentType::NORMALMAP].value[0] = (float)normalColor[0];
				material[Material::ComponentType::NORMALMAP].value[1] = (float)normalColor[1];
				material[Material::ComponentType::NORMALMAP].value[2] = (float)normalColor[2];

				material[Material::ComponentType::NORMALMAP].factor = 1.0f;

				FbxFileTexture* fileTexture = fbx_lambert_p->NormalMap.GetSrcObject<FbxFileTexture>();
				if (fileTexture != nullptr)
				{
					textureFilepath = fileTexture->GetRelativeFileName();
					filepath_t filepath;
					strcpy_s(filepath.data(), filepath.max_size(), textureFilepath);

					material[Material::ComponentType::NORMALMAP].input = filepaths.size();
					filepaths.push_back(filepath);
				}
			}

			materials.push_back(material);

			// -- /extract material from scene --


			return ret_result;
		}
		Result GetAnimationFromFbxScene(
			const FbxScene*				_in_fbxScene_p
			, const uint32_t			_in_elementsToExtract
			, AnimationClip&			_out_animationClip
		) {
			Result result = Result::FAIL;

			FbxScene* fbxScene_p = (FbxScene*)_in_fbxScene_p;
			FbxMesh* fbxMesh_p = nullptr;
			FbxPose* fbxBindPose_p = nullptr;
			uint32_t poseCount = fbxScene_p->GetPoseCount();

			// TODO: split into subfunctions (see GetMeshFromFbxScene)

			// -- extract bind pose from scene --

			for (uint32_t i = 0; i < poseCount; i++)
			{
				FbxPose* fbx_pose_p = fbxScene_p->GetPose(i);
				if (fbx_pose_p->IsBindPose())
				{
					fbxBindPose_p = fbx_pose_p;
					break;
				}
			}

			// -- /extract bind pose from scene --


			// -- get skeleton root from bind pose --

			uint32_t nodeCount = 0;
			FbxNode* fbxNodeRoot_p = nullptr;
			FbxSkeleton* fbxSkeletonRoot_p = nullptr;

			if (fbxBindPose_p == nullptr)
			{
				return result;
			}
			else
			{
				nodeCount = fbxBindPose_p->GetCount();
				FbxNode* fbxNode_p = nullptr;
				FbxSkeleton* fbxSkeleton_p = nullptr;

				for (uint32_t i = 0; i < nodeCount; i++)
				{
					fbxNode_p = fbxBindPose_p->GetNode(i);
					if (fbxNode_p != nullptr)
					{
						fbxSkeleton_p = fbxNode_p->GetSkeleton();
						if (fbxSkeleton_p != nullptr && fbxSkeleton_p->IsSkeletonRoot())
						{
							fbxNodeRoot_p = fbxNode_p;
							fbxSkeletonRoot_p = fbxSkeleton_p;
							break;
						}
					}
				}
			}

			// -- /get skeleton root from bind pose --


			// -- create list of joints from skeleton root --

			std::vector<AnimationJointFbx> jointsFbx;

			AnimationJointFbx jointRoot = { fbxNodeRoot_p, -1 };
			jointsFbx.push_back(jointRoot);

			for (uint32_t i = 0; i < jointsFbx.size(); i++)
			{
				FbxNode* fbxNode_p = jointsFbx[i].fbx_node_p;

				for (int32_t c = 0; c < fbxNode_p->GetChildCount(); c++)
				{
					FbxNode* fbxNodeChild_p = nullptr;
					FbxSkeleton* fbxSkeletonChild_p = nullptr;

					fbxNodeChild_p = fbxNode_p->GetChild(c);

					if (fbxNodeChild_p != nullptr)
						fbxSkeletonChild_p = fbxNodeChild_p->GetSkeleton();

					if (fbxSkeletonChild_p != nullptr)
					{
						AnimationJointFbx childJoint = { fbxNodeChild_p, (int)i };
						jointsFbx.push_back(childJoint);
					}
				}
			}

			// -- /create list of joints from skeleton root --


			// -- convert bind pose joint data --

			std::vector<AnimationJoint> joints_out;

			for (uint32_t i = 0; i < jointsFbx.size(); i++)
			{
				FbxAMatrix transform = jointsFbx[i].fbx_node_p->EvaluateGlobalTransform();

				AnimationJoint joint = { ConvertFbxAMatrixToMatrix(transform), jointsFbx[i].parent_index };

				joints_out.push_back(joint);
			}

			// -- /convert bind pose joint data --


			// -- get animation data from scene --

			FbxAnimStack* fbxAnimStack_p = fbxScene_p->GetCurrentAnimationStack();

			// verify animation stack was extracted
			if (fbxAnimStack_p == nullptr)
			{
				return result;
			}
			else
			{
				FbxTime::EMode mode = FbxTime::EMode::eFrames30;
				FbxTime animDuration = fbxAnimStack_p->GetLocalTimeSpan().GetDuration();
				int64_t frameCount = animDuration.GetFrameCount(mode);

				// store duration in seconds
				_out_animationClip.duration = animDuration.GetSecondDouble();

				for (int64_t i = 1; i < frameCount; i++)  // starts at 1 to skip bind pose at frame 0
				{
					AnimationFrame frame;

					// get keytime for current frame
					FbxTime frameTime;
					frameTime.SetFrame(i, mode);

					// store keytime in seconds
					frame.time = frameTime.GetSecondDouble();

					// get node transforms for current frame
					for (uint32_t n = 0; n < jointsFbx.size(); n++)
						frame.transforms.push_back({ ConvertFbxAMatrixToMatrix(jointsFbx[n].fbx_node_p->EvaluateGlobalTransform(frameTime)) });

					_out_animationClip.frames.push_back(frame);
				}
			}

			// -- /get animation data from scene --

			return result;
		}
#pragma endregion

#pragma region Interface Function Definitions
		bool inline Succeeded(Result _in_r) {
			return static_cast<int>(_in_r) >= static_cast<int>(Result::SUCCESS);
		}

		Result GetMeshFromFbxFile(
			const char*					_in_fbxFilepath
			, const char*				_in_meshName
			, const uint32_t			_in_elementsToExtract
			, Mesh&						_out_mesh
		) {
			Result ret_result = Result::FAIL;

			FbxScene* fbxScene_p = nullptr;
			FbxManager* fbxManager_p = nullptr;

			ret_result = CreateFbxManagerAndImportFbxScene(_in_fbxFilepath, fbxManager_p,
				fbxScene_p);
			if (!Succeeded(ret_result))
			{
				fbxManager_p->Destroy();
				return ret_result;
			}

			ret_result = GetMeshFromFbxScene(fbxScene_p, _in_meshName, _in_elementsToExtract,
				_out_mesh);
			fbxManager_p->Destroy();
			return ret_result;
		}
		Result GetMaterialsFromFbxFile(
			const char*					_in_fbxFilepath
			, const uint32_t			_in_materialNum
			, const uint32_t			_in_elementsToExtract
			, MaterialList&				_out_materialList
		) {
			Result ret_result = Result::FAIL;

			FbxScene* fbxScene_p = nullptr;
			FbxManager* fbxManager_p = nullptr;

			ret_result = CreateFbxManagerAndImportFbxScene(_in_fbxFilepath, fbxManager_p, fbxScene_p);
			if (!Succeeded(ret_result))
			{
				fbxManager_p->Destroy();
				return ret_result;
			}

			ret_result = GetMaterialsFromFbxScene(fbxScene_p, _in_materialNum, _in_elementsToExtract, _out_materialList);
			fbxManager_p->Destroy();
			return ret_result;
		}
		Result GetAnimationFromFbxFile(
			const char*					_in_fbxFilepath
			, const uint32_t			_in_elementsToExtract
			, AnimationClip&			_out_animationClip
		) {
			Result ret_result = Result::FAIL;

			FbxScene* fbxScene_p = nullptr;
			FbxManager* fbxManager_p = nullptr;

			ret_result = CreateFbxManagerAndImportFbxScene(_in_fbxFilepath, fbxManager_p, fbxScene_p);
			if (!Succeeded(ret_result))
			{
				fbxManager_p->Destroy();
				return ret_result;
			}

			ret_result = GetAnimationFromFbxScene(fbxScene_p, _in_elementsToExtract, _out_animationClip);
			fbxManager_p->Destroy();
			return ret_result;
		}
#pragma endregion

	}
}
