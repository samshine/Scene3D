
#include "precomp.h"
#include "model_mesh.h"
#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/SceneEngine/Model/model.h"
#include "Scene3D/SceneEngine/scene_engine_impl.h"

using namespace uicore;

ModelMesh::ModelMesh(SceneEngineImpl *engine, const GraphicContextPtr &gc,  std::shared_ptr<ModelData> model_data) : model_data(model_data)
{
	mesh_buffers.reserve(model_data->meshes.size());
	for (size_t i = 0; i < model_data->meshes.size(); i++)
	{
		ModelMeshBuffers buffers;
		buffers.primitives_array = PrimitivesArray::create(gc);
		buffers.vertices = upload_vector(gc, buffers.primitives_array, 0, model_data->meshes[i].vertices);
		buffers.normals = upload_vector(gc, buffers.primitives_array, 1, model_data->meshes[i].normals);
		buffers.bitangents = upload_vector(gc, buffers.primitives_array, 2, model_data->meshes[i].bitangents);
		buffers.tangents = upload_vector(gc, buffers.primitives_array, 3, model_data->meshes[i].tangents);
		buffers.bone_weights = upload_vector(gc, buffers.primitives_array, 4, model_data->meshes[i].bone_weights, true);
		buffers.bone_selectors = upload_vector(gc, buffers.primitives_array, 5, model_data->meshes[i].bone_selectors, false);
		buffers.colors = upload_vector(gc, buffers.primitives_array, 6, model_data->meshes[i].colors, true);

		for (size_t channel = 0; channel < model_data->meshes[i].channels.size(); channel++)
		{
			buffers.channels.push_back(upload_vector(gc, buffers.primitives_array, 7 + channel, model_data->meshes[i].channels[channel]));
		}

		buffers.elements = ElementArrayVector<unsigned int>(gc, model_data->meshes[i].elements);

		size_t num_materials = model_data->meshes[i].draw_ranges.size();
		for (size_t j = 0; j < num_materials; j++)
		{
			const ModelDataDrawRange &material_range = model_data->meshes[i].draw_ranges[j];

			ModelMaterialUniforms block;

			block.material_ambient = Vec4f(material_range.ambient.get_single_value(), 0.0f);
			block.material_diffuse = Vec4f(material_range.diffuse.get_single_value(), 0.0f);
			block.material_specular = Vec4f(material_range.specular.get_single_value(), 0.0f);
			block.material_glossiness = material_range.glossiness.get_single_value();
			block.material_specular_level = material_range.specular_level.get_single_value();

			block.vectors_per_instance = instance_base_vectors + model_data->bones.size() * vectors_per_bone + num_materials * vectors_per_material;
			block.material_offset = instance_base_vectors + model_data->bones.size() * vectors_per_bone + j * vectors_per_material;

			buffers.uniforms.push_back(UniformVector<ModelMaterialUniforms>(gc, &block, 1));
		}

		mesh_buffers.push_back(buffers);
	}

	for (size_t i = 0; i < model_data->textures.size(); i++)
		textures.push_back(engine->get_texture(gc, model_data->textures[i].name, model_data->textures[i].gamma == 1.0f));
}

int ModelMesh::vectors_per_instance() const
{
	if (model_data->meshes.size() != 1)
		throw Exception("Model::get_vectors_per_instance does not support multiple meshes yet");
	size_t num_materials = model_data->meshes[0].draw_ranges.size();
	return instance_base_vectors + model_data->bones.size() * vectors_per_bone + num_materials * vectors_per_material;
}

template<typename Type>
VertexArrayVector<Type> ModelMesh::upload_vector(const GraphicContextPtr &gc, const PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec)
{
	if (!vec.empty())
	{
		VertexArrayVector<Type> buffer(gc, vec);
		primitives_array->set_attributes(index, buffer);
		return buffer;
	}
	else
	{
		return VertexArrayVector<Type>();
	}
}

template<typename Type>
VertexArrayVector<Type> ModelMesh::upload_vector(const GraphicContextPtr &gc, const PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec, bool normalize)
{
	if (!vec.empty())
	{
		VertexArrayVector<Type> buffer(gc, vec);
		primitives_array->set_attributes(index, buffer, normalize);
		return buffer;
	}
	else
	{
		return VertexArrayVector<Type>();
	}
}
