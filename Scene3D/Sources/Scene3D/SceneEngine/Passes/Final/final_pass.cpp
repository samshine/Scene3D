
#include "precomp.h"
#include "final_pass.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "vertex_present_glsl.h"
#include "vertex_present_hlsl.h"
#include "fragment_present_glsl.h"
#include "fragment_present_hlsl.h"

using namespace uicore;

FinalPass::FinalPass(SceneRender &inout) : inout(inout)
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		present_shader = ShaderSetup::compile(inout.gc, "present", vertex_present_glsl(), fragment_present_glsl(), "");
		present_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		present_shader = ShaderSetup::compile(inout.gc, "present", vertex_present_hlsl(), fragment_present_hlsl(), "");
	}
	if (!present_shader->try_link())
		throw Exception("Shader linking failed!");
	present_shader->bind_attribute_location(0, "PositionInProjection");
	present_shader->set_uniform1i("FinalColors", 0);
	present_shader->set_uniform1i("FinalColorsSampler", 0);

	Vec4f positions[6] =
	{
		Vec4f(-1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f,  1.0f, 1.0f, 1.0f)
	};
	rect_positions = VertexArrayVector<Vec4f>(inout.gc, positions, 6);
	rect_primarray = PrimitivesArray::create(inout.gc);
	rect_primarray->set_attributes(0, rect_positions);

	RasterizerStateDescription rasterizer_desc;
	rasterizer_desc.set_culled(false);
	rasterizer_state = inout.gc->create_rasterizer_state(rasterizer_desc);
}

void FinalPass::run()
{
	ScopeTimeFunction();
	//Texture2DPtr &log_average_light_texture = log_average_light.find_log_average_light(inout.gc, inout.final_color);

	inout.frames.front()->final_color->set_min_filter(filter_nearest);
	inout.frames.front()->final_color->set_mag_filter(filter_nearest);

	if (inout.fb_viewport) inout.gc->set_frame_buffer(inout.fb_viewport);
	inout.gc->set_viewport(inout.viewport, inout.gc->texture_image_y_axis());
	inout.gc->set_texture(0, inout.frames.front()->final_color);
	inout.gc->set_program_object(present_shader);
	inout.gc->set_rasterizer_state(rasterizer_state);
	inout.gc->draw_primitives(type_triangles, 6, rect_primarray);
	inout.gc->reset_program_object();
	inout.gc->reset_texture(0);
	inout.gc->reset_frame_buffer();
}
