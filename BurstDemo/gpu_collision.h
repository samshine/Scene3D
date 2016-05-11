
#pragma once

#include "Collision3D/triangle_mesh_shape.h"

class ModelData;

class GPUCollision
{
public:
	GPUCollision(uicore::GraphicContextPtr gc);

	void set_shape(std::shared_ptr<ModelData> model_data);
	void set_shape(const uicore::Vec3f *vertices, int num_vertices, const unsigned int *elements, int num_elements);

	void update();

	uicore::Texture2DPtr gpu_output_image;

private:
	struct UniformsBlock
	{
		int root;
		int padding1, padding2, padding3;
	};

	uicore::GraphicContextPtr gc;

	uicore::StorageBufferPtr gpu_vertices;
	uicore::StorageBufferPtr gpu_elements;
	uicore::StorageBufferPtr gpu_nodes;
	uicore::UniformBufferPtr gpu_uniforms;

	uicore::ProgramObjectPtr program;
};
