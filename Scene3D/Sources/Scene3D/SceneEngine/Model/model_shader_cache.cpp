
#include "precomp.h"
#include "model_shader_cache.h"
#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "model_render_command.h"
#include "model.h"
#include "model_lod.h"
#include "Scene3D/SceneEngine/Passes/GBuffer/vertex_gbuffer_glsl.h"
#include "Scene3D/SceneEngine/Passes/GBuffer/vertex_gbuffer_hlsl.h"
#include "Scene3D/SceneEngine/Passes/GBuffer/fragment_gbuffer_glsl.h"
#include "Scene3D/SceneEngine/Passes/GBuffer/fragment_gbuffer_hlsl.h"
#include "Scene3D/SceneEngine/Passes/Transparency/vertex_transparency_glsl.h"
#include "Scene3D/SceneEngine/Passes/Transparency/vertex_transparency_hlsl.h"
#include "Scene3D/SceneEngine/Passes/Transparency/fragment_transparency_glsl.h"
#include "Scene3D/SceneEngine/Passes/Transparency/fragment_transparency_hlsl.h"
#include "Scene3D/SceneEngine/Passes/VSMShadowMap/vertex_shadow_glsl.h"
#include "Scene3D/SceneEngine/Passes/VSMShadowMap/vertex_shadow_hlsl.h"
#include "Scene3D/SceneEngine/Passes/VSMShadowMap/fragment_shadow_glsl.h"
#include "Scene3D/SceneEngine/Passes/VSMShadowMap/fragment_shadow_hlsl.h"

using namespace uicore;

ModelShaderCache::Shaderset ModelShaderCache::get_shader(const GraphicContextPtr &gc, const ModelShaderDescription &description)
{
	std::map<ModelShaderDescription, Shaderset>::iterator it = shaders.find(description);
	if (it == shaders.end())
	{
		shaders[description].gbuffer = create_gbuffer_program(gc, description);
		shaders[description].transparency = create_transparency_program(gc, description);
		return shaders[description];
	}
	else
	{
		return it->second;
	}
}

ProgramObjectPtr ModelShaderCache::create_gbuffer_program(const GraphicContextPtr &gc, const ModelShaderDescription &description)
{
	std::string defines;
	if (description.diffuse_channel)
		defines += " DIFFUSE_UV";
	if (description.specular_channel)
		defines += " SPECULAR_UV";
	if (description.bumpmap_channel)
		defines += " BUMPMAP_UV";
	if (description.self_illumination_channel)
		defines += " SI_UV";
	if (description.lightmap_channel)
		defines += " LIGHTMAP_UV";
	if (description.bones)
		defines += " USE_BONES";
	if (description.color_channel)
		defines += " USE_COLORS";

	ProgramObjectPtr gbuffer;
	if (gc->shader_language() == shader_glsl)
		gbuffer = ShaderSetup::compile(gc, "gbuffer", vertex_gbuffer_glsl(), fragment_gbuffer_glsl(), defines);
	else
		gbuffer = ShaderSetup::compile(gc, "gbuffer", vertex_gbuffer_hlsl(), fragment_gbuffer_hlsl(), defines);

	gbuffer->bind_frag_data_location(0, "FragColor");
	gbuffer->bind_frag_data_location(1, "FragFaceNormal");

	gbuffer->bind_frag_data_location(0, "FragDiffuseColor");
	gbuffer->bind_frag_data_location(1, "FragSpecularColor");
	gbuffer->bind_frag_data_location(2, "FragSpecularLevel");
	gbuffer->bind_frag_data_location(3, "FragSelfIllumination");
	gbuffer->bind_frag_data_location(4, "FragNormal");

	gbuffer->bind_attribute_location(0, "AttrPositionInObject");
	gbuffer->bind_attribute_location(1, "AttrNormal");
	gbuffer->bind_attribute_location(2, "AttrBitangent");
	gbuffer->bind_attribute_location(3, "AttrTangent");
	gbuffer->bind_attribute_location(4, "AttrBoneWeights");
	gbuffer->bind_attribute_location(5, "AttrBoneSelectors");
	if (description.color_channel)
		gbuffer->bind_attribute_location(6, "AttrColor");
	if (description.diffuse_channel)
		gbuffer->bind_attribute_location(7, "AttrUVMapA");
	if (description.bumpmap_channel)
		gbuffer->bind_attribute_location(8, "AttrUVMapB");
	if (description.self_illumination_channel)
		gbuffer->bind_attribute_location(9, "AttrUVMapC");
	if (description.specular_channel)
		gbuffer->bind_attribute_location(10, "AttrUVMapD");
	if (description.lightmap_channel)
		gbuffer->bind_attribute_location(11, "AttrUVMapE");

	ShaderSetup::link(gbuffer, "gbuffer program");

	gbuffer->set_uniform1i("InstanceOffsets", 0);
	gbuffer->set_uniform1i("InstanceVectors", 1);
	gbuffer->set_uniform1i("DiffuseTexture", 2);
	gbuffer->set_uniform1i("DiffuseSampler", 2);
	gbuffer->set_uniform1i("BumpMapTexture", 3);
	gbuffer->set_uniform1i("BumpMapSampler", 3);
	gbuffer->set_uniform1i("SelfIlluminationTexture", 4);
	gbuffer->set_uniform1i("SelfIlluminationSampler", 4);
	gbuffer->set_uniform1i("SpecularTexture", 5);
	gbuffer->set_uniform1i("SpecularSampler", 5);
	gbuffer->set_uniform1i("LightMapTexture", 6);
	gbuffer->set_uniform1i("LightMapSampler", 6);
	gbuffer->set_uniform_buffer_index("ModelMaterialUniforms", 0);

	return gbuffer;
}

ProgramObjectPtr ModelShaderCache::create_transparency_program(const GraphicContextPtr &gc, const ModelShaderDescription &description)
{
	std::string defines;
	if (description.diffuse_channel)
		defines += " DIFFUSE_UV";
	if (description.specular_channel)
		defines += " SPECULAR_UV";
	if (description.bumpmap_channel)
		defines += " BUMPMAP_UV";
	if (description.self_illumination_channel)
		defines += " SI_UV";
	if (description.lightmap_channel)
		defines += " LIGHTMAP_UV";
	if (description.bones)
		defines += " USE_BONES";
	if (description.color_channel)
		defines += " USE_COLORS";

	ProgramObjectPtr transparency;
	if (gc->shader_language() == shader_glsl)
		transparency = ShaderSetup::compile(gc, "transparency", vertex_transparency_glsl(), fragment_transparency_glsl(), defines);
	else
		transparency = ShaderSetup::compile(gc, "transparency", vertex_transparency_hlsl(), fragment_transparency_hlsl(), defines);

	transparency->bind_frag_data_location(0, "FragColor");
	transparency->bind_frag_data_location(1, "FragFaceNormal");

	transparency->bind_frag_data_location(0, "FragDiffuseColor");
	transparency->bind_frag_data_location(1, "FragSpecularColor");
	transparency->bind_frag_data_location(2, "FragSpecularLevel");
	transparency->bind_frag_data_location(3, "FragSelfIllumination");
	transparency->bind_frag_data_location(4, "FragNormal");

	transparency->bind_attribute_location(0, "AttrPositionInObject");
	transparency->bind_attribute_location(1, "AttrNormal");
	transparency->bind_attribute_location(2, "AttrBitangent");
	transparency->bind_attribute_location(3, "AttrTangent");
	transparency->bind_attribute_location(4, "AttrBoneWeights");
	transparency->bind_attribute_location(5, "AttrBoneSelectors");
	if (description.color_channel)
		transparency->bind_attribute_location(6, "AttrColor");
	if (description.diffuse_channel)
		transparency->bind_attribute_location(7, "AttrUVMapA");
	if (description.bumpmap_channel)
		transparency->bind_attribute_location(8, "AttrUVMapB");
	if (description.self_illumination_channel)
		transparency->bind_attribute_location(9, "AttrUVMapC");
	if (description.specular_channel)
		transparency->bind_attribute_location(10, "AttrUVMapD");
	if (description.lightmap_channel)
		transparency->bind_attribute_location(11, "AttrUVMapE");

	ShaderSetup::link(transparency, "transparency program");

	transparency->set_uniform1i("InstanceOffsets", 0);
	transparency->set_uniform1i("InstanceVectors", 1);
	transparency->set_uniform1i("DiffuseTexture", 2);
	transparency->set_uniform1i("DiffuseSampler", 2);
	transparency->set_uniform1i("BumpMapTexture", 3);
	transparency->set_uniform1i("BumpMapSampler", 3);
	transparency->set_uniform1i("SelfIlluminationTexture", 4);
	transparency->set_uniform1i("SelfIlluminationSampler", 4);
	transparency->set_uniform1i("SpecularTexture", 5);
	transparency->set_uniform1i("SpecularSampler", 5);
	transparency->set_uniform_buffer_index("ModelMaterialUniforms", 0);

	return transparency;
}

ProgramObjectPtr ModelShaderCache::get_shadow_program(const GraphicContextPtr &gc, bool uses_bones)
{
	if (!uses_bones && shadow_program)
		return shadow_program;
	else if (uses_bones && shadow_bones_program)
		return shadow_bones_program;

	std::string defines;
	if (uses_bones)
		defines += " USE_BONES";

	ProgramObjectPtr program;
	if (gc->shader_language() == shader_glsl)
		program = ShaderSetup::compile(gc, "shadow", vertex_shadow_glsl(), fragment_shadow_glsl(), defines);
	else
		program = ShaderSetup::compile(gc, "shadow", vertex_shadow_hlsl(), fragment_shadow_hlsl(), defines);

	program->bind_frag_data_location(0, "FragMoment");

	program->bind_attribute_location(0, "AttrPositionInObject");
	program->bind_attribute_location(1, "AttrNormal");
	program->bind_attribute_location(2, "AttrBitangent");
	program->bind_attribute_location(3, "AttrTangent");
	program->bind_attribute_location(4, "AttrBoneWeights");
	program->bind_attribute_location(5, "AttrBoneSelectors");

	ShaderSetup::link(program, "shadow program");

	program->set_uniform1i("InstanceOffsets", 0);
	program->set_uniform1i("InstanceVectors", 1);
	program->set_uniform_buffer_index("ModelMaterialUniforms", 0);

	if (!uses_bones)
		shadow_program = program;
	else
		shadow_bones_program = program;

	return program;
}

ProgramObjectPtr ModelShaderCache::get_early_z_program(const GraphicContextPtr &gc, bool uses_bones)
{
	if (!uses_bones && early_z_program)
		return early_z_program;
	else if (uses_bones && early_z_bones_program)
		return early_z_bones_program;

	std::string defines;
	if (uses_bones)
		defines += " USE_BONES";

	ProgramObjectPtr program;
	if (gc->shader_language() == shader_glsl)
		program = ShaderSetup::compile(gc, "early z", vertex_shadow_glsl(), "", defines);
	else
		program = ShaderSetup::compile(gc, "early z", vertex_shadow_hlsl(), "", defines);

	program->bind_frag_data_location(0, "FragMoment");

	program->bind_attribute_location(0, "AttrPositionInObject");
	program->bind_attribute_location(1, "AttrNormal");
	program->bind_attribute_location(2, "AttrBitangent");
	program->bind_attribute_location(3, "AttrTangent");
	program->bind_attribute_location(4, "AttrBoneWeights");
	program->bind_attribute_location(5, "AttrBoneSelectors");

	ShaderSetup::link(program, "early_z program");

	program->set_uniform1i("InstanceOffsets", 0);
	program->set_uniform1i("InstanceVectors", 1);
	program->set_uniform_buffer_index("ModelMaterialUniforms", 0);

	if (!uses_bones)
		early_z_program = program;
	else
		early_z_bones_program = program;

	return program;
}

void ModelShaderCache::create_states(const GraphicContextPtr &gc)
{
	if (!rasterizer_state)
	{
		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_front_face(face_clockwise);
		rasterizer_desc.set_culled(true);
		rasterizer_state = gc->create_rasterizer_state(rasterizer_desc);

		rasterizer_desc.set_culled(false);
		two_sided_rasterizer_state = gc->create_rasterizer_state(rasterizer_desc);
	}
}
