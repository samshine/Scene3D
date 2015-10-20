
#pragma once

#include "model_render_command.h"
#include "model_mesh_buffers.h"

class ModelData;

class ModelLOD
{
public:
	ModelLOD(const uicore::GraphicContextPtr &gc, int model_index, std::shared_ptr<ModelData> model_data);

	std::vector<ModelMeshBuffers> mesh_buffers;

	ModelRenderCommandList gbuffer_commands;
	ModelRenderCommandList transparency_commands;
	ModelRenderCommandList shadow_commands;
	ModelRenderCommandList early_z_commands;

private:
	template<typename Type>
	uicore::VertexArrayVector<Type> upload_vector(const uicore::GraphicContextPtr &gc, const uicore::PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec);

	template<typename Type>
	uicore::VertexArrayVector<Type> upload_vector(const uicore::GraphicContextPtr &gc, const uicore::PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec, bool normalize);
};