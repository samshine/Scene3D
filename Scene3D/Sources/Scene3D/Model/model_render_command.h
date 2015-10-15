
#pragma once

#include "model_mesh_buffers.h"
#include "Scene3D/scene.h"
#include "Scene3D/scene_impl.h"

class ModelRenderCommand
{
public:
	virtual ~ModelRenderCommand() { }
	virtual void execute(Scene_Impl *scene, const uicore::GraphicContextPtr &gc, int num_instances) = 0;
};

class ModelRenderCommandList
{
public:
	~ModelRenderCommandList()
	{
		for (size_t i = 0; i < commands.size(); i++)
			delete commands[i];
	}

	void execute(Scene_Impl *scene, const uicore::GraphicContextPtr &gc, int num_instances)
	{
		for (size_t i = 0; i < commands.size(); i++)
			commands[i]->execute(scene, gc, num_instances);
	}

	std::vector<ModelRenderCommand *> commands;
};

class ModelRenderCommand_BindMeshBuffers : public ModelRenderCommand
{
public:
	ModelRenderCommand_BindMeshBuffers(ModelMeshBuffers *buffers) : buffers(buffers)
	{
	}

	void execute(Scene_Impl *scene, const uicore::GraphicContextPtr &gc, int num_instances)
	{
		gc->set_primitives_array(buffers->primitives_array);
		gc->set_primitives_elements(buffers->elements);
	}

	ModelMeshBuffers *buffers;
};

class ModelRenderCommand_BindShader : public ModelRenderCommand
{
public:
	ModelRenderCommand_BindShader(uicore::ProgramObjectPtr shader) : shader(shader)
	{
	}

	void execute(Scene_Impl *scene, const uicore::GraphicContextPtr &gc, int num_instances)
	{
		gc->set_program_object(shader);
	}

	uicore::ProgramObjectPtr shader;
};

class ModelRenderCommand_BindTexture : public ModelRenderCommand
{
public:
	ModelRenderCommand_BindTexture(int bind_index, Resource<uicore::TexturePtr> texture, uicore::TextureWrapMode wrap_u, uicore::TextureWrapMode wrap_v) : bind_index(bind_index), texture(texture), wrap_u(wrap_u), wrap_v(wrap_v)
	{
	}

	void execute(Scene_Impl *scene, const uicore::GraphicContextPtr &gc, int num_instances)
	{
		std::dynamic_pointer_cast<uicore::Texture2D>(texture.get())->set_wrap_mode(wrap_u, wrap_v);
		gc->set_texture(bind_index, texture.get());
	}

	int bind_index;
	Resource<uicore::TexturePtr> texture;
	uicore::TextureWrapMode wrap_u, wrap_v;
};

class ModelRenderCommand_SetRasterizerStatePtr : public ModelRenderCommand
{
public:
	ModelRenderCommand_SetRasterizerStatePtr(uicore::RasterizerStatePtr state) : state(state)
	{
	}

	void execute(Scene_Impl *scene, const uicore::GraphicContextPtr &gc, int num_instances)
	{
		gc->set_rasterizer_state(state);
	}

	uicore::RasterizerStatePtr state;
};

class ModelRenderCommand_DrawElements : public ModelRenderCommand
{
public:
	ModelRenderCommand_DrawElements(int start_element, int num_elements, uicore::UniformVector<ModelMaterialUniforms> uniforms)
		: start_element(start_element), num_elements(num_elements), uniforms(uniforms)
	{
	}

	void execute(Scene_Impl *scene, const uicore::GraphicContextPtr &gc, int num_instances)
	{
		gc->set_uniform_buffer(0, uniforms);
		gc->draw_primitives_elements_instanced(uicore::type_triangles, num_elements, uicore::type_unsigned_int, start_element * sizeof(unsigned int), num_instances);

		scene->_draw_calls++;
		scene->_triangles_drawn += num_elements / 3;
	}

	int start_element;
	int num_elements;
	uicore::UniformVector<ModelMaterialUniforms> uniforms;
};
