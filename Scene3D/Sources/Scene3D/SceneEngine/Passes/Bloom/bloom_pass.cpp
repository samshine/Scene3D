
#include "precomp.h"
#include "bloom_pass.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_glsl.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_hlsl.h"
#include "fragment_bloom_extract_glsl.h"
#include "fragment_bloom_extract_hlsl.h"
#include "fragment_bloom_combine_glsl.h"
#include "fragment_bloom_combine_hlsl.h"

using namespace uicore;

BloomPass::BloomPass(SceneRender &inout) : inout(inout)
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		extract_shader = ShaderSetup::compile(inout.gc, "bloom extract", vertex_screen_quad_glsl(), fragment_bloom_extract_glsl(), "");
		extract_shader->bind_frag_data_location(0, "FragColor");

		combine_shader = ShaderSetup::compile(inout.gc, "bloom combine", vertex_screen_quad_glsl(), fragment_bloom_combine_glsl(), "");
		combine_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		extract_shader = ShaderSetup::compile(inout.gc, "bloom extract", vertex_screen_quad_hlsl(), fragment_bloom_extract_hlsl(), "");
		combine_shader = ShaderSetup::compile(inout.gc, "bloom combine", vertex_screen_quad_hlsl(), fragment_bloom_combine_hlsl(), "");
	}

	ShaderSetup::link(extract_shader, "bloom extract");
	extract_shader->bind_attribute_location(0, "PositionInProjection");
	extract_shader->set_uniform1i("FinalColors", 0);
	extract_shader->set_uniform1i("FinalColorsSampler", 0);
	extract_shader->set_uniform1i("LogAverageLight", 1);

	ShaderSetup::link(combine_shader, "bloom combine");
	combine_shader->bind_attribute_location(0, "PositionInProjection");
	combine_shader->set_uniform1i("Bloom", 0);
	combine_shader->set_uniform1i("BloomSampler", 0);

	BlendStateDescription add_blend_desc;
	add_blend_desc.enable_blending(true);
	add_blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
	add_blend_state = inout.gc->create_blend_state(add_blend_desc);
}

void BloomPass::run()
{
	ScopeTimeFunction();

	// Extract bloom from render color buffer:
	inout.frames.front()->current_pipeline_texture()->set_min_filter(filter_linear);
	inout.frames.front()->current_pipeline_texture()->set_mag_filter(filter_linear);
	inout.gc->set_frame_buffer(inout.frames.front()->fb_bloom_blurv[0]);
	inout.gc->set_viewport(inout.frames.front()->bloom_blurv[0]->size(), inout.gc->texture_image_y_axis());
	inout.gc->set_texture(0, inout.frames.front()->current_pipeline_texture());
	inout.gc->set_blend_state(inout.no_blend);
	inout.gc->set_program_object(extract_shader);
	inout.gc->draw_primitives(type_triangles, 6, inout.screen_quad_primitives);
	inout.gc->reset_program_object();
	inout.gc->reset_texture(0);
	inout.gc->reset_frame_buffer();

	const float bloom_amount = 4.0f;

	// Blur and downscale:
	for (int i = 0; i < inout.frames.front()->bloom_levels - 1; i++)
	{
		inout.blur.horizontal(inout.gc, bloom_amount, 5, inout.frames.front()->bloom_blurv[i], inout.frames.front()->fb_bloom_blurh[i]);
		inout.blur.vertical(inout.gc, bloom_amount, 5, inout.frames.front()->bloom_blurh[i], inout.frames.front()->fb_bloom_blurv[i + 1]);
	}

	// Upscale and blur:
	for (int i = inout.frames.front()->bloom_levels - 1; i > 0; i--)
	{
		inout.blur.horizontal(inout.gc, bloom_amount, 5, inout.frames.front()->bloom_blurv[i], inout.frames.front()->fb_bloom_blurh[i]);
		inout.blur.vertical(inout.gc, bloom_amount, 5, inout.frames.front()->bloom_blurh[i], inout.frames.front()->fb_bloom_blurv[i]);

		// Linear upscale:
		inout.frames.front()->bloom_blurv[i]->set_min_filter(filter_linear);
		inout.frames.front()->bloom_blurv[i]->set_mag_filter(filter_linear);
		inout.gc->set_frame_buffer(inout.frames.front()->fb_bloom_blurv[i - 1]);
		inout.gc->set_viewport(inout.frames.front()->bloom_blurv[i - 1]->size(), inout.gc->texture_image_y_axis());
		inout.gc->set_texture(0, inout.frames.front()->bloom_blurv[i]);
		inout.gc->set_blend_state(inout.no_blend);
		inout.gc->set_program_object(combine_shader);
		inout.gc->draw_primitives(type_triangles, 6, inout.screen_quad_primitives);
		inout.gc->reset_program_object();
		inout.gc->reset_texture(0);
		inout.gc->reset_frame_buffer();
	}

	inout.blur.horizontal(inout.gc, bloom_amount, 5, inout.frames.front()->bloom_blurv[0], inout.frames.front()->fb_bloom_blurh[0]);
	inout.blur.vertical(inout.gc, bloom_amount, 5, inout.frames.front()->bloom_blurh[0], inout.frames.front()->fb_bloom_blurv[0]);

	// Add bloom to render color buffer:
	inout.frames.front()->bloom_blurv[0]->set_min_filter(filter_linear);
	inout.frames.front()->bloom_blurv[0]->set_mag_filter(filter_linear);
	inout.gc->set_frame_buffer(inout.frames.front()->current_pipeline_fb());
	inout.gc->set_viewport(inout.frames.front()->current_pipeline_texture()->size(), inout.gc->texture_image_y_axis());
	inout.gc->set_texture(0, inout.frames.front()->bloom_blurv[0]);
	inout.gc->set_blend_state(add_blend_state);
	inout.gc->set_program_object(combine_shader);
	inout.gc->draw_primitives(type_triangles, 6, inout.screen_quad_primitives);
	inout.gc->reset_program_object();
	inout.gc->reset_blend_state();
	inout.gc->reset_texture(0);
	inout.gc->reset_frame_buffer();
}
