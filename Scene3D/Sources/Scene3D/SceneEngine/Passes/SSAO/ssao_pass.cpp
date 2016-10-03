
#include "precomp.h"
#include "ssao_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_glsl.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_hlsl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "fragment_depth_blur_glsl.h"
#include "fragment_depth_blur_hlsl.h"
#include "fragment_ssao_glsl.h"
#include "fragment_ssao_hlsl.h"
#include "fragment_ssao_combine_glsl.h"
#include "fragment_ssao_combine_hlsl.h"

using namespace uicore;

SSAOPass::SSAOPass(SceneRender &inout) : inout(inout)
{
	create_ssao_shader();
	create_blur_shader(false);
	create_blur_shader(true);
	create_combine_shader();
}

void SSAOPass::run()
{
	ScopeTimeFunction();
}

void SSAOPass::create_ssao_shader()
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		ssao_shader = ShaderSetup::compile(inout.gc, "ssao", vertex_screen_quad_glsl(), fragment_ssao_glsl(), "USE_RANDOM_TEXTURE");
		ssao_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		ssao_shader = ShaderSetup::compile(inout.gc, "ssao", vertex_screen_quad_hlsl(), fragment_ssao_hlsl(), "USE_RANDOM_TEXTURE");
	}

	ssao_shader->bind_attribute_location(0, "PositionInProjection");
	ssao_shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(ssao_shader, "ssao program");

	ssao_shader->set_uniform_buffer_index("Uniforms", 0);
	ssao_shader->set_uniform1i("DepthTexture", 0);
	ssao_shader->set_uniform1i("DepthTextureSampler", 0);
	ssao_shader->set_uniform1i("RandomTexture", 1);
	ssao_shader->set_uniform1i("RandomTextureSampler", 1);
}

void SSAOPass::create_blur_shader(bool vertical)
{
	auto &blur_shader = vertical ? blur_vertical_shader : blur_horizontal_shader;

	if (inout.gc->shader_language() == shader_glsl)
	{
		blur_shader = ShaderSetup::compile(inout.gc, "depth blur", vertex_screen_quad_glsl(), fragment_depth_blur_glsl(), vertical ? "BLUR_VERTICAL" : "BLUR_HORIZONTAL");
		blur_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		blur_shader = ShaderSetup::compile(inout.gc, "depth blur", vertex_screen_quad_hlsl(), fragment_depth_blur_hlsl(), vertical ? "BLUR_VERTICAL" : "BLUR_HORIZONTAL");
	}

	blur_shader->bind_attribute_location(0, "PositionInProjection");
	blur_shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(blur_shader, "depth blur program");

	blur_shader->set_uniform_buffer_index("Uniforms", 0);
	blur_shader->set_uniform1i("AODepthTexture", 0);
	blur_shader->set_uniform1i("AODepthTextureSampler", 0);
}

void SSAOPass::create_combine_shader()
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		combine_shader = ShaderSetup::compile(inout.gc, "ssao_combine", vertex_screen_quad_glsl(), fragment_ssao_combine_glsl(), "");
		combine_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		combine_shader = ShaderSetup::compile(inout.gc, "ssao_combine", vertex_screen_quad_hlsl(), fragment_ssao_combine_hlsl(), "");
	}

	combine_shader->bind_attribute_location(0, "PositionInProjection");
	combine_shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(combine_shader, "ssao_combine program");

	combine_shader->set_uniform_buffer_index("Uniforms", 0);
	combine_shader->set_uniform1i("AODepthTexture", 0);
	combine_shader->set_uniform1i("AODepthTextureSampler", 0);
}
