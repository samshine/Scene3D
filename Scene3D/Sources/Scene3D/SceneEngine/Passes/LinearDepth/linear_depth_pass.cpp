
#include "precomp.h"
#include "linear_depth_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_glsl.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_hlsl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "fragment_linear_depth_glsl.h"
#include "fragment_linear_depth_hlsl.h"

using namespace uicore;

LinearDepthPass::LinearDepthPass(SceneRender &inout) : inout(inout)
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		shader = ShaderSetup::compile(inout.gc, "linear depth", vertex_screen_quad_glsl(), fragment_linear_depth_glsl(), "");
		shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		shader = ShaderSetup::compile(inout.gc, "linear depth", vertex_screen_quad_hlsl(), fragment_linear_depth_hlsl(), "");
	}

	shader->bind_attribute_location(0, "PositionInProjection");
	shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(shader, "linear depth program");

	shader->set_uniform_buffer_index("Uniforms", 0);
	shader->set_uniform1i("DepthTexture", 0);
	shader->set_uniform1i("ColorTexture", 0);
}

void LinearDepthPass::run()
{
	ScopeTimeFunction();

}
