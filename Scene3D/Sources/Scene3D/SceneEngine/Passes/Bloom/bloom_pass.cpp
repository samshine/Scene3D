
#include "precomp.h"
#include "bloom_pass.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "vertex_bloom_extract_glsl.h"
#include "vertex_bloom_extract_hlsl.h"
#include "fragment_bloom_extract_glsl.h"
#include "fragment_bloom_extract_hlsl.h"
#include "vertex_bloom_combine_glsl.h"
#include "vertex_bloom_combine_hlsl.h"
#include "fragment_bloom_combine_glsl.h"
#include "fragment_bloom_combine_hlsl.h"

using namespace uicore;

BloomPass::BloomPass(SceneRender &inout) : inout(inout)
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		extract_shader = ShaderSetup::compile(inout.gc, "bloom extract", vertex_bloom_extract_glsl(), fragment_bloom_extract_glsl(), "");
		extract_shader->bind_frag_data_location(0, "FragColor");

		combine_shader = ShaderSetup::compile(inout.gc, "bloom combine", vertex_bloom_combine_glsl(), fragment_bloom_combine_glsl(), "");
		combine_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		extract_shader = ShaderSetup::compile(inout.gc, "bloom extract", vertex_bloom_extract_hlsl(), fragment_bloom_extract_hlsl(), "");
		combine_shader = ShaderSetup::compile(inout.gc, "bloom combine", vertex_bloom_combine_hlsl(), fragment_bloom_combine_hlsl(), "");
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

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	blend_state = inout.gc->create_blend_state(blend_desc);

	BlendStateDescription add_blend_desc;
	add_blend_desc.enable_blending(true);
	add_blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
	add_blend_state = inout.gc->create_blend_state(add_blend_desc);
}

void BloomPass::run()
{
	ScopeTimeFunction();

	// Extract bloom from render color buffer:
	inout.final_color->set_min_filter(filter_linear);
	inout.final_color->set_mag_filter(filter_linear);
	inout.gc->set_frame_buffer(inout.fb_bloom_blurv[0]);
	inout.gc->set_viewport(inout.bloom_blurv[0]->size(), inout.gc->texture_image_y_axis());
	inout.gc->set_texture(0, inout.final_color);
	inout.gc->set_blend_state(blend_state);
	inout.gc->set_program_object(extract_shader);
	inout.gc->draw_primitives(type_triangles, 6, rect_primarray);
	inout.gc->reset_program_object();
	inout.gc->reset_texture(0);
	inout.gc->reset_frame_buffer();

	const float bloom_amount = 4.0f;

	// Blur and downscale:
	for (int i = 0; i < inout.bloom_levels - 1; i++)
	{
		inout.blur.horizontal(inout.gc, bloom_amount, 5, inout.bloom_blurv[i], inout.fb_bloom_blurh[i]);
		inout.blur.vertical(inout.gc, bloom_amount, 5, inout.bloom_blurh[i], inout.fb_bloom_blurv[i + 1]);
	}

	// Upscale and blur:
	for (int i = inout.bloom_levels - 1; i > 0; i--)
	{
		inout.blur.horizontal(inout.gc, bloom_amount, 5, inout.bloom_blurv[i], inout.fb_bloom_blurh[i]);
		inout.blur.vertical(inout.gc, bloom_amount, 5, inout.bloom_blurh[i], inout.fb_bloom_blurv[i]);

		// Linear upscale:
		inout.bloom_blurv[i]->set_min_filter(filter_linear);
		inout.bloom_blurv[i]->set_mag_filter(filter_linear);
		inout.gc->set_frame_buffer(inout.fb_bloom_blurv[i - 1]);
		inout.gc->set_viewport(inout.bloom_blurv[i - 1]->size(), inout.gc->texture_image_y_axis());
		inout.gc->set_texture(0, inout.bloom_blurv[i]);
		inout.gc->set_blend_state(blend_state);
		inout.gc->set_program_object(combine_shader);
		inout.gc->draw_primitives(type_triangles, 6, rect_primarray);
		inout.gc->reset_program_object();
		inout.gc->reset_texture(0);
		inout.gc->reset_frame_buffer();
	}

	inout.blur.horizontal(inout.gc, bloom_amount, 5, inout.bloom_blurv[0], inout.fb_bloom_blurh[0]);
	inout.blur.vertical(inout.gc, bloom_amount, 5, inout.bloom_blurh[0], inout.fb_bloom_blurv[0]);

	// Add bloom to render color buffer:
	inout.bloom_blurv[0]->set_min_filter(filter_linear);
	inout.bloom_blurv[0]->set_mag_filter(filter_linear);
	inout.gc->set_frame_buffer(inout.fb_final_color);
	inout.gc->set_viewport(inout.final_color->size(), inout.gc->texture_image_y_axis());
	inout.gc->set_texture(0, inout.bloom_blurv[0]);
	inout.gc->set_blend_state(add_blend_state);
	inout.gc->set_program_object(combine_shader);
	inout.gc->draw_primitives(type_triangles, 6, rect_primarray);
	inout.gc->reset_program_object();
	inout.gc->reset_blend_state();
	inout.gc->reset_texture(0);
	inout.gc->reset_frame_buffer();
}
