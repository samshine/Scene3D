
#include "precomp.h"
#include "fbx_model_loader.h"
#include "AssetCompiler/FBXModel/fbx_model.h"
#include "AssetCompiler/ModelDescription/model_desc.h"
#include "AssetCompiler/ModelDescription/model_desc_animation.h"
#include "AssetCompiler/ModelDescription/model_desc_attachment_point.h"
#include "AssetCompiler/ModelDescription/model_desc_particle_emitter.h"
#include "AssetCompiler/ModelDescription/model_desc_material.h"
#include "fbx_model_impl.h"
#include <algorithm>

#pragma comment(lib, "libfbxsdk-mt.lib")

using namespace uicore;

FBXModelLoader::FBXModelLoader(FBXModelImpl *model, const ModelDesc &desc)
	: model_desc(desc), model(model), model_data(std::make_shared<ModelData>())
{
	convert_node(model->scene->GetRootNode());

	if (unskinned_bone_needed)
	{
		unsigned char unskinned_bone = (unsigned char)bones.size();
		for (auto &mesh : model_data->meshes)
		{
			for (auto &selectors : mesh.bone_selectors)
			{
				selectors.x = std::min(selectors.x, unskinned_bone);
				selectors.y = std::min(selectors.y, unskinned_bone);
				selectors.z = std::min(selectors.z, unskinned_bone);
				selectors.w = std::min(selectors.w, unskinned_bone);
			}
		}
	}

	for (const auto &anim : model_desc.animations)
	{
		convert_bones(anim);
	}

	for (const auto &fbx_attachment : model_desc.attachment_points)
	{
		int bone_index = find_bone_index(fbx_attachment.bone_name);
		if (bone_index != -1)
		{
			ModelDataAttachmentPoint attachment;
			attachment.name = fbx_attachment.name;
			attachment.position = fbx_attachment.position;
			attachment.orientation = fbx_attachment.orientation;
			attachment.bone_selector = bone_index;
			model_data->attachment_points.push_back(attachment);
		}
	}
}

int FBXModelLoader::find_bone_index(const std::string &name)
{
	for (size_t i = 0; i < bones.size(); i++)
	{
		if (bones[i].bone_node->GetName() == name)
			return (int)i;
	}

	return -1;
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
	mesh_to_world = Mat4f::scale(1.0f, 1.0f, -1.0f) * mesh_to_world; // Flip Z axis
	Mat3f normal_mesh_to_world = Mat3f(mesh_to_world).inverse().transpose();

	VertexMappingVector vertices(mesh->GetControlPointsCount());
	std::vector<VertexMapping *> elements;

	convert_polygons(node, mesh, vertices, elements, mesh_to_world, normal_mesh_to_world);
	convert_skins(node, mesh, vertices);

	if (model_data->meshes.empty())
		model_data->meshes.resize(1);

	ModelDataMesh &model_mesh = model_data->meshes[0];

	model_mesh.channels.resize(4);

	for (size_t i = 0; i < vertices.size(); i++)
	{
		for (VertexMapping *mapping = vertices[i]; mapping != nullptr; mapping = mapping->next)
		{
			mapping->vertex_index = (int)model_mesh.vertices.size();

			model_mesh.vertices.push_back(mapping->position);
			//model_mesh.colors.push_back(mapping->color);
			model_mesh.normals.push_back(mapping->normal);
			model_mesh.bone_weights.push_back(mapping->bone_weights);
			model_mesh.bone_selectors.push_back(mapping->bone_selectors);
			model_mesh.channels[0].push_back(mapping->diffuse_uv);
			model_mesh.channels[1].push_back(mapping->specular_uv);
			model_mesh.channels[2].push_back(mapping->normal_uv);
			model_mesh.channels[3].push_back(mapping->emission_uv);
		}
	}

	std::vector<std::vector<VertexMapping *> > material_elements(node->GetMaterialCount());

	if (node->GetMaterialCount() > 0)
	{
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
	}
	else
	{
		material_elements.push_back(elements);
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

		model_mesh.draw_ranges.push_back(create_draw_range(start_element, num_elements, node->GetMaterial((int)material_index)));
	}
}

ModelDataDrawRange FBXModelLoader::create_draw_range(size_t start_element, size_t num_elements, FbxSurfaceMaterial *material)
{
	ModelDataDrawRange range;
	range.start_element = (int)start_element;
	range.num_elements = (int)num_elements;
	range.two_sided = false;
	range.transparent = false;
	range.alpha_test = false;

	if (material == 0)
	{
		range.ambient.set_single_value(Vec3f(1.0f));
		range.diffuse.set_single_value(Vec3f(1.0f));
		range.specular.set_single_value(Vec3f(1.0f));
		range.specular_level.set_single_value(0.0f);
		range.glossiness.set_single_value(0.0f);
		range.self_illumination_amount.set_single_value(0.0f);
		range.self_illumination.set_single_value(Vec3f(0.0f));
		return range;
	}

	std::string material_name = material->GetName();

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

		range.ambient.set_single_value(ambient * ambient_factor);
		range.diffuse.set_single_value(diffuse * diffuse_factor);
		range.self_illumination_amount.set_single_value(1.0f);
		range.self_illumination.set_single_value(emissive * emissive_factor);

		if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			FbxSurfacePhong *phong = static_cast<FbxSurfacePhong*>(lambert);

			FbxDouble3 specular = phong->Specular.Get();
			float spec_level = (float)phong->SpecularFactor.Get();
			float glossiness = (float)phong->Shininess.Get();
			//FbxDouble3 reflection = phong->Reflection.Get();
			//float reflection_factor = (float)phong->ReflectionFactor.Get();

			range.specular.set_single_value(to_vec3f(specular));
			range.specular_level.set_single_value(spec_level);
			range.glossiness.set_single_value(glossiness);
		}
		else
		{
			range.specular.set_single_value(Vec3f());
			range.specular_level.set_single_value(0.0f);
			range.glossiness.set_single_value(0.0f);
		}

		range.diffuse_map = create_texture_channel(0, FbxSurfaceMaterial::sDiffuse, material, 2.2f);
		range.specular_map = create_texture_channel(1, FbxSurfaceMaterial::sSpecular, material, 1.0f);
		range.bumpmap_map = create_texture_channel(2, FbxSurfaceMaterial::sNormalMap, material, 1.0f);
		range.self_illumination_map = create_texture_channel(3, FbxSurfaceMaterial::sEmissive, material, 2.2f);
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

	for (const auto &desc_material : model_desc.materials)
	{
		if (desc_material.mesh_material == material_name)
		{
			range.alpha_test = desc_material.alpha_test;
			range.two_sided = desc_material.two_sided;
			range.transparent = desc_material.transparent;
		}
	}

	return range;
}

ModelDataTextureMap FBXModelLoader::create_texture_channel(int channel, const char *property_name, FbxSurfaceMaterial *material, float gamma)
{
	ModelDataTextureMap map;

	FbxFileTexture *texture = 0;

	if (material->FindProperty(property_name).GetSrcObjectCount<FbxLayeredTexture>() > 0)
	{
		FbxLayeredTexture *layer = material->FindProperty(property_name).GetSrcObject<FbxLayeredTexture>(0);
		if (layer->FindProperty(property_name).GetSrcObjectCount<FbxFileTexture>() > 0)
			texture = layer->FindProperty(property_name).GetSrcObject<FbxFileTexture>(0);
	}
	else if (material->FindProperty(property_name).GetSrcObjectCount<FbxFileTexture>() > 0)
	{
		texture = material->FindProperty(property_name).GetSrcObject<FbxFileTexture>(0);
	}

	if (texture)
	{
		std::string filename = PathHelp::combine(model->base_path, PathHelp::filename(texture->GetFileName()));
		std::string uv_set = texture->UVSet.Get();

		//Vec3f translate = to_vec3f(texture->Translation.Get());

		FbxDouble3 r = texture->Rotation.Get();
		//Quaternionf rotation((float)r[0], (float)r[1], (float)r[2], angle_degrees, order_XYZ);

		//Vec3f scale = to_vec3f(texture->Scaling.Get());

		map.texture = (int)model_data->textures.size();

		map.channel = channel;
		map.wrap_x = texture->WrapModeU.Get() == FbxTexture::eRepeat ? ModelDataTextureMap::wrap_repeat : ModelDataTextureMap::wrap_clamp_to_edge;
		map.wrap_y = texture->WrapModeV.Get() == FbxTexture::eRepeat ? ModelDataTextureMap::wrap_repeat : ModelDataTextureMap::wrap_clamp_to_edge;

		/*if (texture->UVSwap.Get())
		{
			rotation = rotation * Quaternionf(0.0f, 0.0f, 90.0f, angle_degrees, order_XYZ);
			scale.y = -scale.y;
		}*/

		//map.uvw_offset.set_single_value(translate);
		//map.uvw_rotation.set_single_value(rotation);
		//map.uvw_scale.set_single_value(scale);

		model_data->textures.push_back(ModelDataTexture(filename, gamma));
	}

	return map;
}

void FBXModelLoader::convert_polygons(FbxNode *node, FbxMesh *mesh, VertexMappingVector &vertices, std::vector<VertexMapping *> &face_vertices, const Mat4f &mesh_to_world, const Mat3f &normal_mesh_to_world)
{
	mesh->GenerateNormals();

	auto scaling = node->EvaluateGlobalTransform().GetS() * node->GetGeometricScaling(FbxNode::eSourcePivot);
	bool neg_scale = scale_dir(scaling[0]) * scale_dir(scaling[1]) * scale_dir(scaling[2]) < 0.0;

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
			< 0.0f;
		if (neg_scale)
			ccw = !ccw;

		for (int point = 0; point < num_points; point++)
		{
			int ccw_point = ccw ? num_points - point - 1 : point;
			int control_index = mesh->GetPolygonVertex(poly, ccw_point);

			Vec3f position = Vec3f(mesh_to_world * Vec4f(Vec3f(to_vec4f(control_points[control_index])), 1.0f));
			//Vec4ub color = get_color(mesh, control_index, vertex_index + ccw_point);
			Vec3f normal = normal_mesh_to_world * get_normal(mesh, control_index, vertex_index + ccw_point);
			Vec2f diffuse_uv = get_uv(mesh, control_index, vertex_index + ccw_point, 0);

			normal.normalize();

			//diffuse_uv = Vec2f(1.0f) - diffuse_uv; // Seems to be needed for Blender
			diffuse_uv.y = 1.0f - diffuse_uv.y; // Seems to be needed for Max

			if (vertices[control_index] == nullptr)
			{
				vertices[control_index] = new VertexMapping();
				vertices[control_index]->position = position;
				//vertices[control_index]->color = color;
				vertices[control_index]->normal = normal;
				vertices[control_index]->diffuse_uv = diffuse_uv;
				vertices[control_index]->specular_uv = diffuse_uv;
				vertices[control_index]->normal_uv = diffuse_uv;
				vertices[control_index]->emission_uv = diffuse_uv;

				face_vertices.push_back(vertices[control_index]);
			}
			else
			{
				VertexMapping *mapping = vertices[control_index];
				while (mapping->position != position /*|| mapping->color != color*/ || mapping->normal != normal || mapping->diffuse_uv != diffuse_uv)
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
						//mapping->color = color;
						mapping->normal = normal;
						mapping->diffuse_uv = diffuse_uv;
						mapping->specular_uv = diffuse_uv;
						mapping->normal_uv = diffuse_uv;
						mapping->emission_uv = diffuse_uv;
						break;
					}
				}

				face_vertices.push_back(mapping);
			}
		}

		vertex_index += num_points;
	}
}

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

			SkinnedBone bone;
			bone.bone_node = cluster->GetLink();
			cluster->GetTransformLinkMatrix(bone.bind_bone_to_world);
			bones.push_back(bone);

			int bone_selector = ((int)bones.size()) - 1;

			int num_indices = cluster->GetControlPointIndicesCount();
			int *indices = cluster->GetControlPointIndices();
			double *weights = cluster->GetControlPointWeights();

			for (int i = 0; i < num_indices; i++)
			{
				int cp_index = indices[i];
				if (cp_index < 0 || cp_index >= (int)vertices.size())
					continue;

				for (VertexMapping *mapping = vertices[cp_index]; mapping != nullptr; mapping = mapping->next)
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

			bool unskinned_vertex = true;
			for (unsigned int j = 0; j < 4; j++)
			{
				if (bone_selectors[j] != 255)
					unskinned_vertex = false;
			}

			if (unskinned_vertex)
			{
				unskinned_bone_needed = true;
				bone_weights[0] = 255;
				bone_weights[1] = 0;
				bone_weights[2] = 0;
				bone_weights[3] = 0;
			}
			else 
			{
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

	// Directional lights with a 0,0,0 rotation vector points in the -Y direction. Ours point in the +Z direction.
	Quaternionf directionRotation(-90.0f, 0.0f, 0.0f, angle_degrees, order_YXZ);

	// Is a light "geometric"? Nobody knows!
	FbxAMatrix inverseZ = FbxAMatrix(FbxVector4(0.0, 0.0, 0.0, 1.0), FbxVector4(0.0, 0.0, 0.0, 1.0), FbxVector4(1.0, 1.0, -1.0, 1.0));
	FbxAMatrix light_to_world = inverseZ * (node->EvaluateGlobalTransform() * FbxAMatrix(node->GetGeometricTranslation(FbxNode::eSourcePivot), node->GetGeometricRotation(FbxNode::eSourcePivot), node->GetGeometricScaling(FbxNode::eSourcePivot)));

	// light_to_world.GetS() returns a negative value(!!), so maybe the above calculations are not valid. If only the FBX SDK had been more clear about its math..
	auto scale_vector = node->EvaluateGlobalTransform().GetS() * node->GetGeometricScaling(FbxNode::eSourcePivot);
	float scale = (float)std::max({ scale_vector[0], scale_vector[1], scale_vector[2] });

	Vec3f position = Vec3f(to_vec4f(light_to_world.GetT()));
	Quaternionf orientation = to_quaternionf(light_to_world.GetQ()) * directionRotation;

	ModelDataLight model_light;

	model_light.position.set_single_value(position);
	model_light.orientation.set_single_value(orientation);

	switch (light->LightType.Get())
	{
		case FbxLight::ePoint:
			// To do: add a light type to ModelDataLight
			model_light.hotspot.set_single_value(0.0f);
			model_light.falloff.set_single_value(0.0f);
			break;

		case FbxLight::eSpot:
			model_light.hotspot.set_single_value((float)light->InnerAngle.Get());
			model_light.falloff.set_single_value((float)light->OuterAngle.Get());
			break;

		case FbxLight::eDirectional:
		case FbxLight::eArea:
		case FbxLight::eVolume:
		default:
			return;
	}

	//model_light.casts_light = light->CastLight.Get();
	model_light.casts_shadows = light->CastShadows.Get();

	model_light.color.set_single_value(to_vec3f(light->Color.Get()) * (float)(light->Intensity.Get() * 0.01));
	//model_light.shadow_color.set_single_value(light->ShadowColor.Get());

	model_light.attenuation_start.set_single_value((float)light->FarAttenuationStart.Get() * scale);
	model_light.attenuation_end.set_single_value((float)light->FarAttenuationEnd.Get() * scale);

	model_light.aspect.set_single_value(1.0f);
	model_light.ambient_illumination.set_single_value(0.0f);
	model_light.rectangle = false;

	// std::string gobo_filename = light->FileName.Get();
	// FbxTexture *shadow_texture = light->GetShadowTexture();
	// bool ground_projection = light->DrawGroundProjection.Get();
	// bool volumetric_projection = light->DrawVolumetricLight.Get();
	// bool front_volumetric_projection = light->DrawFrontFacingVolumetricLight.Get();
	// float fog = (float)light->Fog.Get();

	// std::string name = light->GetName();

	model_data->lights.push_back(model_light);
}

void FBXModelLoader::convert_bones(const ModelDescAnimation &animation_desc)
{
	model_data->bones.reserve(bones.size() + 1);
	model_data->bones.resize(bones.size());

	FbxScene *animation_scene = nullptr;
	if (!animation_desc.fbx_filename.empty())
		animation_scene = model->get_animation_scene(animation_desc.fbx_filename);
	if (animation_scene == nullptr)
		animation_scene = model->scene;

	FbxTime::EMode time_mode = animation_scene->GetGlobalSettings().GetTimeMode();
	FbxTimeSpan timespan;

	FbxAnimStack *anim_stack = animation_scene->GetSrcObject<FbxAnimStack>(0);
	if (anim_stack)
	{
		FbxTakeInfo *take = animation_scene->GetTakeInfo(anim_stack->GetName());
		if (take)
			timespan = take->mLocalTimeSpan;
		else
			animation_scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timespan);
	}
	else
	{
		animation_scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timespan);
	}

	float scene_start_time = (float)timespan.GetStart().GetSecondDouble();
	float scene_stop_time = (float)timespan.GetStop().GetSecondDouble();

	int start_frame = animation_desc.start_frame;
	int end_frame = animation_desc.end_frame;

	if (end_frame == -1)
		end_frame = (int)(timespan.GetStop() - timespan.GetStart()).GetFrameCount(time_mode) - 1;

	end_frame = std::max(end_frame, start_frame);

	float start_time = (float)(scene_start_time + start_frame / FbxTime::GetFrameRate(time_mode));
	float stop_time = (float)(scene_start_time + end_frame / FbxTime::GetFrameRate(time_mode));
	float stop_time_loop = (float)(scene_start_time + (end_frame + 1) / FbxTime::GetFrameRate(time_mode));

	FbxAnimEvaluator *scene_evaluator = animation_scene->GetAnimationEvaluator();

	ModelDataAnimation animation;
	animation.name = animation_desc.name;
	animation.length = animation_desc.loop ? (stop_time_loop - start_time) : (stop_time - start_time);
	animation.loop = animation_desc.loop;
	animation.playback_speed = animation_desc.play_speed;
	animation.moving_speed = animation_desc.move_speed;
	animation.rarity = animation_desc.rarity;
	model_data->animations.push_back(animation);

	size_t timeline_index = model_data->animations.size() - 1;

	for (int frame = start_frame; frame <= end_frame + 1; frame++)
	{
		float step_time = (float)(scene_start_time + frame / FbxTime::GetFrameRate(time_mode));

		FbxTime current_time;
		current_time.SetSecondDouble((frame != end_frame + 1) ? step_time : start_time);

		for (size_t bone_index = 0; bone_index < bones.size(); bone_index++)
		{
			const auto &fbx_bone = bones[bone_index];
			auto &model_bone = model_data->bones[bone_index];

			FbxNode *bone_node = nullptr;
			if (animation_scene != model->scene)
				bone_node = animation_scene->FindNodeByName(fbx_bone.bone_node->GetName());
			if (bone_node == nullptr)
				bone_node = fbx_bone.bone_node;

			FbxAMatrix bone_to_world = scene_evaluator->GetNodeGlobalTransform(bone_node, current_time, FbxNode::eSourcePivot, true);
			FbxAMatrix bind_bone_to_world = fbx_bone.bind_bone_to_world;

			// Flip Z axis
			bone_to_world = FbxAMatrix(FbxVector4(0.0, 0.0, 0.0), FbxVector4(0.0, 0.0, 0.0), FbxVector4(1.0, 1.0, -1.0)) * bone_to_world;
			bind_bone_to_world = FbxAMatrix(FbxVector4(0.0, 0.0, 0.0), FbxVector4(0.0, 0.0, 0.0), FbxVector4(1.0, 1.0, -1.0)) * bind_bone_to_world;

			Vec3f bind_position = Vec3f(to_mat4f(bind_bone_to_world) * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
			Quaternionf bind_rotation = to_quaternionf(bind_bone_to_world.GetQ()).inverse();

			Vec3f position = Vec3f(to_mat4f(bone_to_world) * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
			Quaternionf orientation = to_quaternionf(bone_to_world.GetQ()) * bind_rotation;

			model_bone.pivot = bind_position;

			model_bone.position.timelines.resize(timeline_index + 1);
			model_bone.orientation.timelines.resize(timeline_index + 1);

			model_bone.position.timelines[timeline_index].timestamps.push_back(step_time - start_time);
			model_bone.position.timelines[timeline_index].values.push_back(position);

			model_bone.orientation.timelines[timeline_index].timestamps.push_back(step_time - start_time);
			model_bone.orientation.timelines[timeline_index].values.push_back(orientation);
		}

		// FbxMatrix &object_to_world = scene_evaluator->GetNodeGlobalTransform(node, current_time);
		// FbxAnimCurveNode &value = scene_evaluator->GetPropertyValue(value, current_time);
	}

	if (unskinned_bone_needed && !bones.empty())
	{
		ModelDataBone model_bone;
		model_bone.pivot = Vec3f();
		model_bone.position.set_single_value(Vec3f());
		model_bone.orientation.set_single_value(Quaternionf());
		model_data->bones.push_back(model_bone);
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

Vec4f FBXModelLoader::to_vec4f(const FbxColor &c)
{
	return Vec4f((float)c.mRed, (float)c.mGreen, (float)c.mBlue, (float)c.mAlpha);
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

Quaternionf FBXModelLoader::to_quaternionf(const FbxQuaternion &q)
{
	return Quaternionf((float)q.GetAt(3), (float)q.GetAt(0), (float)q.GetAt(1), (float)q.GetAt(2));
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
