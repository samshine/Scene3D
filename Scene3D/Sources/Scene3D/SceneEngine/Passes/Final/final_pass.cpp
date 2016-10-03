
#include "precomp.h"
#include "final_pass.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_glsl.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_hlsl.h"
#include "fragment_present_glsl.h"
#include "fragment_present_hlsl.h"

using namespace uicore;

FinalPass::FinalPass(SceneRender &inout) : inout(inout)
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		present_shader = ShaderSetup::compile(inout.gc, "present", vertex_screen_quad_glsl(), fragment_present_glsl(), "");
		present_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		present_shader = ShaderSetup::compile(inout.gc, "present", vertex_screen_quad_hlsl(), fragment_present_hlsl(), "");
	}
	if (!present_shader->try_link())
		throw Exception("Shader linking failed!");
	present_shader->bind_attribute_location(0, "PositionInProjection");
	present_shader->set_uniform1i("FinalColors", 0);
	present_shader->set_uniform1i("FinalColorsSampler", 0);
}

void FinalPass::run()
{
	ScopeTimeFunction();
	//Texture2DPtr &log_average_light_texture = log_average_light.find_log_average_light(inout.gc, inout.final_color);

	inout.frames.front()->current_pipeline_texture()->set_min_filter(filter_nearest);
	inout.frames.front()->current_pipeline_texture()->set_mag_filter(filter_nearest);

	if (inout.fb_viewport) inout.gc->set_frame_buffer(inout.fb_viewport);
	inout.gc->set_viewport(inout.final_viewport, inout.gc->texture_image_y_axis());
	inout.gc->set_texture(0, inout.frames.front()->current_pipeline_texture());
	inout.gc->set_program_object(present_shader);
	inout.gc->set_blend_state(inout.no_blend);
	inout.gc->set_rasterizer_state(inout.no_cull_rasterizer);
	inout.gc->draw_primitives(type_triangles, 6, inout.screen_quad_primitives);
	inout.gc->reset_program_object();
	inout.gc->reset_texture(0);
	inout.gc->reset_frame_buffer();
}
