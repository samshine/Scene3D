
#pragma once

#include "model_shader_description.h"
#include "Scene3D/ModelData/model_data.h"

class Model;

class ModelShaderCache
{
public:
	ModelShaderCache(const std::string &shader_path);

	void create_gbuffer_commands(uicore::GraphicContext &gc, Model *model, int level);
	void create_transparency_commands(uicore::GraphicContext &gc, Model *model, int level);
	void create_shadow_commands(uicore::GraphicContext &gc, Model *model, int level);
	void create_early_z_commands(uicore::GraphicContext &gc, Model *model, int level);

private:
	struct Shaderset
	{
		uicore::ProgramObject gbuffer;
		uicore::ProgramObject transparency;
	};

	Shaderset get_shader(uicore::GraphicContext &gc, const ModelShaderDescription &description);
	uicore::ProgramObject create_gbuffer_program(uicore::GraphicContext &gc, const ModelShaderDescription &description);
	uicore::ProgramObject create_transparency_program(uicore::GraphicContext &gc, const ModelShaderDescription &description);
	uicore::ProgramObject get_shadow_program(uicore::GraphicContext &gc, bool uses_bones);
	uicore::ProgramObject get_early_z_program(uicore::GraphicContext &gc, bool uses_bones);
	void create_states(uicore::GraphicContext &gc);
	static uicore::TextureWrapMode to_wrap_mode(ModelDataTextureMap::WrapMode mode);

	std::string base_path;
	std::map<ModelShaderDescription, Shaderset> shaders;
	uicore::ProgramObject shadow_program, shadow_bones_program;
	uicore::ProgramObject early_z_program, early_z_bones_program;
	uicore::RasterizerState rasterizer_state;
	uicore::RasterizerState two_sided_rasterizer_state;
};
