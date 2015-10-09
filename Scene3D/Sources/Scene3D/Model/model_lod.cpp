
#include "precomp.h"
#include "model_lod.h"
#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/Model/model.h"

using namespace uicore;

ModelLOD::ModelLOD(const GraphicContextPtr &gc, int model_index, std::shared_ptr<ModelData> model_data)
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

			block.model_index = model_index;
			block.vectors_per_instance = Model::instance_base_vectors + model_data->bones.size() * Model::vectors_per_bone + num_materials * Model::vectors_per_material;
			block.material_offset = Model::instance_base_vectors + model_data->bones.size() * Model::vectors_per_bone + j * Model::vectors_per_material;

			buffers.uniforms.push_back(UniformVector<ModelMaterialUniforms>(gc, &block, 1));
		}

		mesh_buffers.push_back(buffers);
	}
}

template<typename Type>
VertexArrayVector<Type> ModelLOD::upload_vector(const GraphicContextPtr &gc, const PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec)
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
VertexArrayVector<Type> ModelLOD::upload_vector(const GraphicContextPtr &gc, const PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec, bool normalize)
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
