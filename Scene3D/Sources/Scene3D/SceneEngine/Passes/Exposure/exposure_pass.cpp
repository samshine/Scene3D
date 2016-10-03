
#include "precomp.h"
#include "exposure_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_glsl.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_hlsl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "fragment_exposure_extract_glsl.h"
#include "fragment_exposure_average_glsl.h"
#include "fragment_exposure_combine_glsl.h"
#include "fragment_exposure_extract_hlsl.h"
#include "fragment_exposure_average_hlsl.h"
#include "fragment_exposure_combine_hlsl.h"

using namespace uicore;

ExposurePass::ExposurePass(SceneRender &inout) : inout(inout)
{
	create_extract_shader();
	create_average_shader();
	create_combine_shader();
}

void ExposurePass::run()
{
	ScopeTimeFunction();
}

void ExposurePass::create_extract_shader()
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		extract_shader = ShaderSetup::compile(inout.gc, "exposure extract", vertex_screen_quad_glsl(), fragment_exposure_extract_glsl(), "");
		extract_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		extract_shader = ShaderSetup::compile(inout.gc, "exposure extract", vertex_screen_quad_hlsl(), fragment_exposure_extract_hlsl(), "");
	}

	extract_shader->bind_attribute_location(0, "PositionInProjection");
	extract_shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(extract_shader, "exposure extract program");

	extract_shader->set_uniform_buffer_index("Uniforms", 0);
	extract_shader->set_uniform1i("SceneTexture", 0);
	extract_shader->set_uniform1i("SceneTextureSampler", 0);
}

void ExposurePass::create_average_shader()
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		average_shader = ShaderSetup::compile(inout.gc, "exposure average", vertex_screen_quad_glsl(), fragment_exposure_average_glsl(), "");
		average_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		average_shader = ShaderSetup::compile(inout.gc, "exposure average", vertex_screen_quad_hlsl(), fragment_exposure_average_hlsl(), "");
	}

	average_shader->bind_attribute_location(0, "PositionInProjection");
	average_shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(average_shader, "exposure average program");

	average_shader->set_uniform_buffer_index("Uniforms", 0);
	average_shader->set_uniform1i("ExposureTexture", 0);
}

void ExposurePass::create_combine_shader()
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		combine_shader = ShaderSetup::compile(inout.gc, "exposure combine", vertex_screen_quad_glsl(), fragment_exposure_combine_glsl(), "");
		combine_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		combine_shader = ShaderSetup::compile(inout.gc, "exposure combine", vertex_screen_quad_hlsl(), fragment_exposure_combine_hlsl(), "");
	}

	combine_shader->bind_attribute_location(0, "PositionInProjection");
	combine_shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(combine_shader, "exposure combine program");

	combine_shader->set_uniform_buffer_index("Uniforms", 0);
	combine_shader->set_uniform1i("ExposureTexture", 0);
	combine_shader->set_uniform1i("ExposureTextureSampler", 0);
}
