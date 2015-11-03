
#pragma once

#include "model_shader_description.h"
#include "Scene3D/ModelData/model_data.h"

class Model;

class ModelShaderCache
{
public:
	void create_gbuffer_commands(const uicore::GraphicContextPtr &gc, Model *model, int level);
	void create_transparency_commands(const uicore::GraphicContextPtr &gc, Model *model, int level);
	void create_shadow_commands(const uicore::GraphicContextPtr &gc, Model *model, int level);
	void create_early_z_commands(const uicore::GraphicContextPtr &gc, Model *model, int level);

private:
	struct Shaderset
	{
		uicore::ProgramObjectPtr gbuffer;
		uicore::ProgramObjectPtr transparency;
	};

	Shaderset get_shader(const uicore::GraphicContextPtr &gc, const ModelShaderDescription &description);
	uicore::ProgramObjectPtr create_gbuffer_program(const uicore::GraphicContextPtr &gc, const ModelShaderDescription &description);
	uicore::ProgramObjectPtr create_transparency_program(const uicore::GraphicContextPtr &gc, const ModelShaderDescription &description);
	uicore::ProgramObjectPtr get_shadow_program(const uicore::GraphicContextPtr &gc, bool uses_bones);
	uicore::ProgramObjectPtr get_early_z_program(const uicore::GraphicContextPtr &gc, bool uses_bones);
	void create_states(const uicore::GraphicContextPtr &gc);
	static uicore::TextureWrapMode to_wrap_mode(ModelDataTextureMap::WrapMode mode);

	std::map<ModelShaderDescription, Shaderset> shaders;
	uicore::ProgramObjectPtr shadow_program, shadow_bones_program;
	uicore::ProgramObjectPtr early_z_program, early_z_bones_program;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::RasterizerStatePtr two_sided_rasterizer_state;
};
