
#include "precomp.h"
#include "fbx_model_loader.h"
#include "AssetCompiler/FBXModel/fbx_model.h"
#include "AssetCompiler/FBXModel/animation.h"
#include "fbx_model_impl.h"
#include <algorithm>

#pragma comment(lib, "libfbxsdk-mt.lib")

namespace clan
{
	FBXModelLoader::FBXModelLoader(FBXModelImpl *model, const std::vector<FBXAnimation> &animations, const std::vector<FBXAttachmentPoint> &attachment_points, const std::vector<FBXParticleEmitter> &emitters)
		: model(model), model_data(std::make_shared<ModelData>())
	{
		convert_node(model->scene->GetRootNode());
		for (const auto &anim : animations)
		{
			convert_bones(anim);
		}
	}

	void FBXModelLoader::convert_node(FbxNode *node)
	{
		FbxNodeAttribute *node_attribute = node->GetNodeAttribute();
		if (node_attribute)
		{
			FbxNodeAttribute::EType node_type = node_attribute->GetAttributeType();
			switch (node_type)
			{
			case FbxNodeAttribute::eMesh:
				convert_mesh(node);
				break;

			case FbxNodeAttribute::eCamera:
				convert_camera(node);
				break;

			case FbxNodeAttribute::eLight:
				convert_light(node);

			case FbxNodeAttribute::eMarker:
			case FbxNodeAttribute::eSkeleton:
			case FbxNodeAttribute::eNurbs:
			case FbxNodeAttribute::ePatch:
			case FbxNodeAttribute::eLODGroup:
				break;
			}
		}

		// FbxProperty user_property = node->GetFirstProperty();
		// bool has_user_properties = user_property.IsValid();

		for (int i = 0; i < node->GetChildCount(); i++)
		{
			convert_node(node->GetChild(i));
		}
	}

	void FBXModelLoader::convert_mesh(FbxNode *node)
	{
		FbxMesh *mesh = static_cast<FbxMesh*>(node->GetNodeAttribute());

		Mat4f mesh_to_world = to_mat4f(node->EvaluateGlobalTransform() * FbxAMatrix(node->GetGeometricTranslation(FbxNode::eSourcePivot), node->GetGeometricRotation(FbxNode::eSourcePivot), node->GetGeometricScaling(FbxNode::eSourcePivot)));
		Mat3f normal_mesh_to_world = Mat3f(mesh_to_world).inverse().transpose();

		VertexMappingVector vertices(mesh->GetControlPointsCount());
		std::vector<VertexMapping *> elements;

		convert_polygons(mesh, vertices, elements, mesh_to_world, normal_mesh_to_world);
		convert_skins(node, mesh, vertices);

		if (model_data->meshes.empty())
			model_data->meshes.resize(1);

		ModelDataMesh &model_mesh = model_data->meshes[0];

		model_mesh.channels.resize(4);

		for (size_t i = 0; i < vertices.size(); i++)
		{
			for (VertexMapping *mapping = vertices[i]; mapping != nullptr; mapping = mapping->next)
			{
				mapping->vertex_index = model_mesh.vertices.size();

				model_mesh.vertices.push_back(mapping->position);
				model_mesh.colors.push_back(mapping->color);
				model_mesh.normals.push_back(mapping->normal);
				model_mesh.tangents.push_back(mapping->tangent);
				model_mesh.bitangents.push_back(mapping->bitangent);
				model_mesh.bone_weights.push_back(mapping->bone_weights);
				model_mesh.bone_selectors.push_back(mapping->bone_selectors);
				model_mesh.channels[0].push_back(mapping->diffuse_uv);
				model_mesh.channels[1].push_back(mapping->specular_uv);
				model_mesh.channels[2].push_back(mapping->normal_uv);
				model_mesh.channels[3].push_back(mapping->emission_uv);
			}
		}

		std::vector<std::vector<VertexMapping *> > material_elements(node->GetMaterialCount());

		for (int element_index = 0; element_index < mesh->GetElementMaterialCount(); element_index++)
		{
			FbxGeometryElementMaterial *element = mesh->GetElementMaterial(element_index);
			if (element->GetMappingMode() == FbxGeometryElement::eByPolygon)
			{
				int index_array_size = element->GetIndexArray().GetCount();
				for (int index = 0; index < index_array_size; index++)
				{
					int material_index = element->GetIndexArray().GetAt(index);

					material_elements[material_index].insert(material_elements[material_index].end(), elements.begin() + index * 3, elements.begin() + index * 3 + 3);
				}
			}
			else if (element->GetMappingMode() == FbxGeometryElement::eAllSame)
			{
				int material_index = element->GetIndexArray().GetAt(0);
				material_elements[material_index].insert(material_elements[material_index].end(), elements.begin(), elements.end());
			}
		}

		for (size_t material_index = 0; material_index < material_elements.size(); material_index++)
		{
			size_t start_element = model_mesh.elements.size();
			size_t num_elements = material_elements[material_index].size();

			if (num_elements == 0)
				continue;

			for (size_t i = 0; i < num_elements; i++)
			{
				model_mesh.elements.push_back(material_elements[material_index][i]->vertex_index);
			}

			model_mesh.draw_ranges.push_back(create_draw_range(start_element, num_elements, node->GetMaterial(material_index)));
		}
	}

	ModelDataDrawRange FBXModelLoader::create_draw_range(size_t start_element, size_t num_elements, FbxSurfaceMaterial *material)
	{
		ModelDataDrawRange range;
		range.start_element = start_element;
		range.num_elements = num_elements;

		// To do: where can we retrieve these flags from?
		range.two_sided = false;
		range.transparent = false;
		range.alpha_test = false;

		if (material->GetClassId().Is(FbxSurfaceLambert::ClassId) || material->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			FbxSurfaceLambert *lambert = static_cast<FbxSurfaceLambert*>(material);

			Vec3f emissive = to_vec3f(lambert->Emissive.Get());
			float emissive_factor = (float)lambert->EmissiveFactor.Get();

			Vec3f ambient = to_vec3f(lambert->Ambient.Get());
			float ambient_factor = (float)lambert->AmbientFactor.Get();

			Vec3f diffuse = to_vec3f(lambert->Diffuse.Get());
			float diffuse_factor = (float)lambert->DiffuseFactor.Get();

			Vec3f normal_map = to_vec3f(lambert->NormalMap.Get());

			Vec3f bump = to_vec3f(lambert->Bump.Get());
			float bump_factor = (float)lambert->BumpFactor.Get();

			Vec3f transparent_color = to_vec3f(lambert->TransparentColor.Get());
			float transparency_factor = (float)lambert->TransparencyFactor.Get();

			Vec3f displacement_color = to_vec3f(lambert->DisplacementColor.Get());
			float displacement_factor = (float)lambert->DisplacementFactor.Get();

			Vec3f vector_displacement_color = to_vec3f(lambert->VectorDisplacementColor.Get());
			float vector_displacement_factor = (float)lambert->VectorDisplacementFactor.Get();

			// Force emissive/self-illumination to zero until they are better understood.
			emissive_factor = 0.0f;
			emissive = Vec3f();

			range.ambient.set_single_value(ambient * ambient_factor);
			range.diffuse.set_single_value(diffuse * diffuse_factor);
			range.self_illumination_amount.set_single_value(emissive_factor);
			range.self_illumination.set_single_value(emissive * 0.25f); // To do: fix this properly (3ds max SI only can do 0-1 range, and we need more to allow blooming, grr..)

			if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				FbxSurfacePhong *phong = static_cast<FbxSurfacePhong*>(lambert);

				FbxDouble3 specular = phong->Specular.Get();
				float factor = (float)phong->SpecularFactor.Get();
				float shininess = (float)phong->Shininess.Get();
				FbxDouble3 reflection = phong->Reflection.Get();
				float reflection_factor = (float)phong->ReflectionFactor.Get();

				// To do: find the shader math (haha, good luck!) for Autodesk FBX phong and make sure this is correct:

				range.specular.set_single_value(to_vec3f(specular));
				range.specular_level.set_single_value(factor);
				range.glossiness.set_single_value(shininess);
			}
			else
			{
				range.specular.set_single_value(Vec3f());
				range.specular_level.set_single_value(0.0f);
				range.glossiness.set_single_value(0.0f);
			}

			// Get diffuse texture:
			const char *property_name = FbxSurfaceMaterial::sDiffuse;
			if (material->FindProperty(property_name).GetSrcObjectCount<FbxFileTexture>() > 0)
			{
				FbxFileTexture *texture = material->FindProperty(property_name).GetSrcObject<FbxFileTexture>(0);
				std::string filename = PathHelp::combine(model->base_path, PathHelp::get_filename(texture->GetFileName()));
				std::string uv_set = texture->UVSet.Get();

				Vec3f translate = to_vec3f(texture->Translation.Get());

				FbxDouble3 r = texture->Rotation.Get();
				Quaternionf rotation((float)r[0], (float)r[1], (float)r[2], angle_degrees, order_XYZ);

				Vec3f scale = to_vec3f(texture->Scaling.Get());

				range.diffuse_map.texture = model_data->textures.size();

				range.diffuse_map.channel = 0;
				range.diffuse_map.wrap_x = texture->WrapModeU.Get() == FbxTexture::eRepeat ? ModelDataTextureMap::wrap_repeat : ModelDataTextureMap::wrap_clamp_to_edge;
				range.diffuse_map.wrap_y = texture->WrapModeV.Get() == FbxTexture::eRepeat ? ModelDataTextureMap::wrap_repeat : ModelDataTextureMap::wrap_clamp_to_edge;

				if (texture->UVSwap.Get())
				{
					rotation = rotation * Quaternionf(0.0f, 0.0f, 90.0f, angle_degrees, order_XYZ);
					scale.y = -scale.y;
				}

				range.diffuse_map.uvw_offset.set_single_value(translate);
				range.diffuse_map.uvw_rotation.set_single_value(rotation);
				range.diffuse_map.uvw_scale.set_single_value(scale);

				model_data->textures.push_back(ModelDataTexture(filename, 2.2f));
			}
		}
		else
		{
			// Unknown material class id

			range.ambient.set_single_value(Vec3f(1.0f));
			range.diffuse.set_single_value(Vec3f(1.0f));
			range.specular.set_single_value(Vec3f(1.0f));
			range.specular_level.set_single_value(0.0f);
			range.glossiness.set_single_value(0.0f);
			range.self_illumination_amount.set_single_value(0.0f);
			range.self_illumination.set_single_value(Vec3f(0.0f));
		}

		return range;
	}

	void FBXModelLoader::convert_polygons(FbxMesh *mesh, VertexMappingVector &vertices, std::vector<VertexMapping *> &face_vertices, const Mat4f &mesh_to_world, const Mat3f &normal_mesh_to_world)
	{
		mesh->GenerateNormals();

		FbxVector4 *control_points = mesh->GetControlPoints();

		int num_polygons = mesh->GetPolygonCount();

		face_vertices.reserve(num_polygons * 3);

		int vertex_index = 0;
		for (int poly = 0; poly < num_polygons; poly++)
		{
			int num_points = mesh->GetPolygonSize(poly);
			if (num_points != 3)
				throw Exception("FBX loader can only handle triangle faces");

			Vec3f points[3] = {
				Vec3f(mesh_to_world * Vec4f(Vec3f(to_vec4f(control_points[mesh->GetPolygonVertex(poly, 0)])), 1.0f)),
				Vec3f(mesh_to_world * Vec4f(Vec3f(to_vec4f(control_points[mesh->GetPolygonVertex(poly, 1)])), 1.0f)),
				Vec3f(mesh_to_world * Vec4f(Vec3f(to_vec4f(control_points[mesh->GetPolygonVertex(poly, 2)])), 1.0f))
			};

			bool ccw = Vec3f::dot(
				Vec3f::cross(points[1] - points[0], points[2] - points[0]),
				normal_mesh_to_world * get_normal(mesh, mesh->GetPolygonVertex(poly, 0), vertex_index))
				>= 0.0f;

			for (int point = 0; point < num_points; point++)
			{
				int ccw_point = ccw ? point : num_points - point - 1;
				int control_index = mesh->GetPolygonVertex(poly, ccw_point);

				Vec3f position = Vec3f(mesh_to_world * Vec4f(Vec3f(to_vec4f(control_points[control_index])), 1.0f));
				Vec4ub color = get_color(mesh, control_index, vertex_index + ccw_point);
				Vec3f normal = normal_mesh_to_world * get_normal(mesh, control_index, vertex_index + ccw_point);
				Vec3f tangent = get_tangent(mesh, control_index, vertex_index + ccw_point);
				Vec3f bitangent = get_bitangent(mesh, control_index, vertex_index + ccw_point);
				Vec2f diffuse_uv = get_uv(mesh, control_index, vertex_index + ccw_point, 0);

				//diffuse_uv = Vec2f(1.0f) - diffuse_uv; // Seems to be needed for Blender
				diffuse_uv.y = 1.0f - diffuse_uv.y; // Seems to be needed for Max

				if (vertices[control_index] == nullptr)
				{
					vertices[control_index] = new VertexMapping();
					vertices[control_index]->position = position;
					vertices[control_index]->color = color;
					vertices[control_index]->normal = normal;
					vertices[control_index]->tangent = tangent;
					vertices[control_index]->bitangent = bitangent;
					vertices[control_index]->diffuse_uv = diffuse_uv;

					face_vertices.push_back(vertices[control_index]);
				}
				else
				{
					VertexMapping *mapping = vertices[control_index];
					while (mapping->position != position || mapping->color != color || mapping->normal != normal || mapping->tangent != tangent || mapping->bitangent != bitangent || mapping->diffuse_uv != diffuse_uv)
					{
						if (mapping->next)
						{
							mapping = mapping->next;
						}
						else
						{
							mapping->next = new VertexMapping();
							mapping = mapping->next;

							mapping->position = position;
							mapping->color = color;
							mapping->normal = normal;
							mapping->tangent = tangent;
							mapping->bitangent = bitangent;
							mapping->diffuse_uv = diffuse_uv;
							break;
						}
					}

					face_vertices.push_back(mapping);
				}
			}

			vertex_index += num_points;
		}
	}

	/*
		bind_node_to_world = pCluster->GetTransformMatrix();
		// Multiply bind_mesh_to_world by Geometric Transformation
		mesh_to_node = node->GeometryXX
		bind_mesh_to_world = mesh_to_node * bind_node_to_world;

		// Get the link initial global position and the link current global position.
		bind_bone_to_world = pCluster->GetTransformLinkMatrix();
		bone_to_world = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		bind_mesh_to_bind_bone = bind_bone_to_world.Inverse() * bind_mesh_to_world;

		// Compute the current position of the link relative to the reference.
		bone_to_mesh = mesh_to_world.Inverse() * bone_to_world;

		// Compute the shift of the link relative to the reference.
		bind_mesh_to_mesh = bone_to_mesh * bind_mesh_to_bind_bone;
		*/

	void FBXModelLoader::convert_skins(FbxNode *node, FbxMesh *mesh, VertexMappingVector &vertices)
	{
		int num_skins = mesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int skin_index = 0; skin_index < num_skins; skin_index++)
		{
			FbxSkin *skin = static_cast<FbxSkin *>(mesh->GetDeformer(skin_index, FbxDeformer::eSkin));

			FbxSkin::EType skinning_type = skin->GetSkinningType();
			if (skinning_type != FbxSkin::eLinear && skinning_type != FbxSkin::eRigid)
				throw Exception("Only linear or rigid skinning types supported");

			int num_clusters = skin->GetClusterCount();
			for (int cluster_index = 0; cluster_index < num_clusters; cluster_index++)
			{
				FbxCluster *cluster = skin->GetCluster(cluster_index);

				if (cluster->GetLinkMode() != FbxCluster::eNormalize)
					throw Exception("Unsupported skinning link mode");

				if (!cluster->GetLink())
					throw Exception("Skin cluster with no link is not supported");

				std::string link_name = cluster->GetLink()->GetName();

				FbxAMatrix bind_node_to_world;
				cluster->GetTransformMatrix(bind_node_to_world);

				FbxAMatrix mesh_to_node(node->GetGeometricTranslation(FbxNode::eSourcePivot), node->GetGeometricRotation(FbxNode::eSourcePivot), node->GetGeometricScaling(FbxNode::eSourcePivot));

				FbxAMatrix bind_mesh_to_world = bind_node_to_world * mesh_to_node;

				FbxAMatrix bind_bone_to_world;
				cluster->GetTransformLinkMatrix(bind_bone_to_world);

				FbxAMatrix mesh_to_world = node->EvaluateGlobalTransform() * mesh_to_node;

				int bone_selector = bones.size();
				SkinnedBone bone;
				bone.bone_node = cluster->GetLink();
				bone.bind_mesh_to_world = to_mat4f(bind_mesh_to_world);
				bone.bind_bone_to_world = to_mat4f(bind_bone_to_world);
				bone.mesh_to_world = to_mat4f(mesh_to_world);
				bones.push_back(bone);

				int num_indices = cluster->GetControlPointIndicesCount();
				int *indices = cluster->GetControlPointIndices();
				double *weights = cluster->GetControlPointWeights();

				for (int i = 0; i < num_indices; i++)
				{
					for (VertexMapping *mapping = vertices[indices[i]]; mapping != nullptr; mapping = mapping->next)
					{
						unsigned char *bone_selectors = reinterpret_cast<unsigned char*>(&mapping->bone_selectors);
						unsigned char *bone_weights = reinterpret_cast<unsigned char*>(&mapping->bone_weights);
						for (unsigned int j = 0; j < 4; j++)
						{
							if (bone_selectors[j] == 255)
							{
								bone_selectors[j] = bone_selector;
								bone_weights[j] = clamp((int)(weights[i] * 255 + 0.5), 0, 255);
								break;
							}
						}
					}
				}
			}
		}

		for (size_t i = 0; i < vertices.size(); i++)
		{
			for (VertexMapping *mapping = vertices[i]; mapping != nullptr; mapping = mapping->next)
			{
				int weight_sum = 0;
				unsigned int max_sum = 0;
				unsigned int max_index = 0;
				unsigned char *bone_selectors = reinterpret_cast<unsigned char*>(&mapping->bone_selectors);
				unsigned char *bone_weights = reinterpret_cast<unsigned char*>(&mapping->bone_weights);
				for (unsigned int j = 0; j < 4; j++)
				{
					if (bone_selectors[j] == 255)
					{
						bone_selectors[j] = 0;
					}

					if (bone_weights[j] > max_sum)
					{
						max_sum = bone_weights[j];
						max_index = j;
					}

					weight_sum += bone_weights[j];
				}
				if (weight_sum != 255)
					bone_weights[max_index] = bone_weights[max_index] + (255 - weight_sum);
			}
		}
	}

	Vec4ub FBXModelLoader::get_color(FbxMesh *mesh, int control_index, int vertex_index)
	{
		FbxGeometryElementVertexColor *element = mesh->GetElementVertexColor(0);
		if (element)
		{
			if (element->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return to_vec4ub(element->GetDirectArray().GetAt(control_index));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(control_index);
					return to_vec4ub(element->GetDirectArray().GetAt(id));
				}
			}
			else if (element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return to_vec4ub(element->GetDirectArray().GetAt(vertex_index));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(vertex_index);
					return to_vec4ub(element->GetDirectArray().GetAt(id));
				}
			}
		}
		return Vec4ub();
	}

	Vec3f FBXModelLoader::get_normal(FbxMesh *mesh, int control_index, int vertex_index)
	{
		FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
		if (element)
		{
			if (element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(vertex_index)));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(vertex_index);
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(id)));
				}
			}
			else if (element->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(control_index)));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(control_index);
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(id)));
				}
			}
			else
			{
				throw Exception("Unsupported normal type");
			}
		}
		return Vec3f();
	}

	Vec3f FBXModelLoader::get_tangent(FbxMesh *mesh, int control_index, int vertex_index)
	{
		FbxGeometryElementTangent *element = mesh->GetElementTangent(0);
		if (element)
		{
			if (element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(vertex_index)));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(vertex_index);
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(id)));
				}
			}
			else
			{
				throw Exception("Unsupported tangent type");
			}
		}
		return Vec3f();
	}

	Vec3f FBXModelLoader::get_bitangent(FbxMesh *mesh, int control_index, int vertex_index)
	{
		FbxGeometryElementBinormal *element = mesh->GetElementBinormal(0);
		if (element)
		{
			if (element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(vertex_index)));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(vertex_index);
					return Vec3f(to_vec4f(element->GetDirectArray().GetAt(id)));
				}
			}
			else
			{
				throw Exception("Unsupported bitangent type");
			}
		}
		return Vec3f();
	}

	Vec2f FBXModelLoader::get_uv(FbxMesh *mesh, int control_index, int vertex_index, int uv_channel_index)
	{
		FbxGeometryElementUV *element = mesh->GetElementUV(uv_channel_index);
		if (element)
		{
			if (element->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return to_vec2f(element->GetDirectArray().GetAt(control_index));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(control_index);
					return to_vec2f(element->GetDirectArray().GetAt(id));
				}
			}
			else if (element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				if (element->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					return to_vec2f(element->GetDirectArray().GetAt(vertex_index));
				}
				else if (element->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = element->GetIndexArray().GetAt(vertex_index);
					return to_vec2f(element->GetDirectArray().GetAt(id));
				}
			}
			else
			{
				throw Exception("Unsupported bitangent type");
			}
		}

		return Vec2f();
	}

	void FBXModelLoader::convert_camera(FbxNode *node)
	{
		FbxCamera *camera = static_cast<FbxCamera*>(node->GetNodeAttribute());
		/*
			std::string name = camera->GetName();
			FbxCamera::EProjectionType projection_type = camera->ProjectionType.Get(); // Perspective, Orthogonal
			FbxDouble3 position = camera->Position.Get();
			*/
	}

	void FBXModelLoader::convert_light(FbxNode *node)
	{
		FbxLight *light = static_cast<FbxLight*>(node->GetNodeAttribute());
		/*
			std::string name = light->GetName();

			FbxLight::EType type = light->LightType.Get(); // point, directional, spot
			bool casts_light = light->CastLight.Get();
			bool casts_shadows = light->CastShadows.Get();

			std::string light_texture = light->FileName.Get();
			bool ground_projection = light->DrawGroundProjection.Get();
			bool volumetric_projection = light->DrawVolumetricLight.Get();
			bool front_volumetric_projection = light->DrawFrontFacingVolumetricLight.Get();

			FbxDouble3 color = light->Color.Get();
			float intensity = (float)light->Intensity.Get();
			float outer_angle = (float)light->OuterAngle.Get();
			float inner_angle = (float)light->InnerAngle.Get();
			float fog = (float)light->Fog.Get();
			*/
	}

	void FBXModelLoader::convert_bones(const FBXAnimation &animation_desc)
	{
		model_data->bones.resize(bones.size());

		FbxTime::EMode time_mode = model->scene->GetGlobalSettings().GetTimeMode();
		FbxTimeSpan timespan;

		model->scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timespan);
		float scene_start_time = (float)timespan.GetStart().GetSecondDouble();
		float scene_stop_time = (float)timespan.GetStop().GetSecondDouble();

		float start_time = (float)(scene_start_time + animation_desc.start_frame / FbxTime::GetFrameRate(time_mode));
		float stop_time = (float)(scene_start_time + animation_desc.end_frame / FbxTime::GetFrameRate(time_mode));

		start_time = clan::clamp(start_time, scene_start_time, scene_stop_time);
		stop_time = clan::clamp(stop_time, scene_start_time, scene_stop_time);
		stop_time = std::max(stop_time, start_time);

		FbxAnimEvaluator *scene_evaluator = model->scene->GetAnimationEvaluator();

		size_t timeline_index = model_data->animations.size();

		ModelDataAnimation animation;
		animation.name = animation_desc.name;
		animation.length = stop_time - start_time;
		animation.loop = animation_desc.loop;
		animation.playback_speed = animation_desc.play_speed;
		animation.moving_speed = animation_desc.move_speed;
		animation.rarity = animation_desc.rarity;
		model_data->animations.push_back(animation);

		int num_steps = (int)std::ceil((stop_time - start_time) * 60.0f);
		for (int step = 0; step <= num_steps; step++)
		{
			float step_time = std::min(start_time + step / 60.0f, stop_time);

			FbxTime current_time;
			current_time.SetSecondDouble(step_time);

			for (size_t bone_index = 0; bone_index < bones.size(); bone_index++)
			{
				Mat4f bone_to_world = to_mat4f(scene_evaluator->GetNodeGlobalTransform(bones[bone_index].bone_node, current_time, FbxNode::eSourcePivot, true));
				const Mat4f &bind_mesh_to_world = bones[bone_index].bind_mesh_to_world;
				const Mat4f &bind_bone_to_world = bones[bone_index].bind_bone_to_world;
				const Mat4f &mesh_to_world = bones[bone_index].mesh_to_world;

				// bind mesh -> (bind) world -> bind bone -> (current bone) world -> bind mesh -> (current mesh) world
				Mat4f skinned_bone_to_world = mesh_to_world * Mat4f::inverse(bind_mesh_to_world) * bone_to_world * Mat4f::inverse(bind_bone_to_world) * bind_mesh_to_world * Mat4f::inverse(mesh_to_world);

				Vec3f position, scale;
				Quaternionf orientation;
				skinned_bone_to_world.decompose(position, orientation, scale);
				orientation.normalize();

				model_data->bones[bone_index].position.timelines.resize(timeline_index + 1);
				model_data->bones[bone_index].orientation.timelines.resize(timeline_index + 1);
				model_data->bones[bone_index].scale.timelines.resize(timeline_index + 1);

				model_data->bones[bone_index].position.timelines[timeline_index].timestamps.push_back(step_time - start_time);
				model_data->bones[bone_index].position.timelines[timeline_index].values.push_back(position);

				model_data->bones[bone_index].orientation.timelines[timeline_index].timestamps.push_back(step_time - start_time);
				model_data->bones[bone_index].orientation.timelines[timeline_index].values.push_back(orientation);

				model_data->bones[bone_index].scale.timelines[timeline_index].timestamps.push_back(step_time - start_time);
				model_data->bones[bone_index].scale.timelines[timeline_index].values.push_back(scale);
			}

			// FbxMatrix &object_to_world = scene_evaluator->GetNodeGlobalTransform(node, current_time);
			// FbxAnimCurveNode &value = scene_evaluator->GetPropertyValue(value, current_time);
		}
	}

	Vec2f FBXModelLoader::to_vec2f(const FbxVector2 &v)
	{
		return Vec2f((float)v[0], (float)v[1]);
	}

	Vec3f FBXModelLoader::to_vec3f(const FbxDouble3 &d)
	{
		return Vec3f((float)d[0], (float)d[1], (float)d[2]);
	}

	Vec4f FBXModelLoader::to_vec4f(const FbxVector4 &v)
	{
		return Vec4f((float)v[0], (float)v[1], (float)v[2], (float)v[3]);
	}

	Vec4ub FBXModelLoader::to_vec4ub(const FbxColor &c)
	{
		return Vec4ub(clamp((int)(c.mRed * 255 + 0.5), 0, 255), clamp((int)(c.mGreen * 255 + 0.5), 0, 255), clamp((int)(c.mBlue * 255 + 0.5), 0, 255), clamp((int)(c.mAlpha * 255 + 0.5), 0, 255));
	}

	Mat4f FBXModelLoader::to_mat4f(const FbxAMatrix &m)
	{
		const double *src_matrix = m;
		Mat4f matrix;
		for (int i = 0; i < 16; i++)
			matrix[i] = (float)src_matrix[i];
		return matrix;
	}
}

/*
	// Global settings:

	const FbxGlobalSettings &globals = scene->GetGlobalSettings();

	Colorf scene_ambient((float)globals.GetAmbientColor().mRed, (float)globals.GetAmbientColor().mGreen, (float)globals.GetAmbientColor().mBlue, (float)globals.GetAmbientColor().mAlpha);
	std::string default_camera = globals.GetDefaultCamera();
*/

/*
int get_group(FbxMesh *mesh, int polygon)
{
	int num_groups = mesh->GetElementPolygonGroupCount();
	for (int group = 0; group < num_groups; group++)
	{
		FbxGeometryElementPolygonGroup *poly_group = mesh->GetElementPolygonGroup(group);
		if (poly_group->GetMappingMode() == FbxGeometryElement::eByPolygon && poly_group->GetReferenceMode() == FbxGeometryElement::eIndex)
		{
			return poly_group->GetIndexArray().GetAt(polygon);
		}
	}
	return -1;
}
*/

/*
	// Get 3ds max user properties:
	FbxProperty p = m_node->FindProperty("UDP3DSMAX");
	if (p.IsValid()) FbxString str = s.Get<FbxString>();
*/
