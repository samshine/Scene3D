
#include "precomp.h"
#include "bloom_pass.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/shader_setup.h"

using namespace uicore;

BloomPass::BloomPass(const GraphicContextPtr &gc, const std::string &shader_path, SceneRender &inout) : inout(inout)
{
	if (gc->shader_language() == shader_glsl)
	{
		bloom_shader = ShaderSetup::compile(gc, "", PathHelp::combine(shader_path, "Final/vertex_present.glsl"), PathHelp::combine(shader_path, "Bloom/fragment_bloom_extract.glsl"), "");
		bloom_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		bloom_shader = ShaderSetup::compile(gc, "", PathHelp::combine(shader_path, "Final/vertex_present.hlsl"), PathHelp::combine(shader_path, "Bloom/fragment_bloom_extract.hlsl"), "");
	}

	ShaderSetup::link(bloom_shader, "bloom extract program");

	bloom_shader->bind_attribute_location(0, "PositionInProjection");
	bloom_shader->set_uniform1i("FinalColors", 0);
	bloom_shader->set_uniform1i("FinalColorsSampler", 0);
	bloom_shader->set_uniform1i("LogAverageLight", 1);

	Vec4f positions[6] =
	{
		Vec4f(-1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f,  1.0f, 1.0f, 1.0f)
	};
	rect_positions = VertexArrayVector<Vec4f>(gc, positions, 6);
	rect_primarray = PrimitivesArray::create(gc);
	rect_primarray->set_attributes(0, rect_positions);

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	blend_state = gc->create_blend_state(blend_desc);
}

void BloomPass::run(const GraphicContextPtr &gc, SceneImpl *scene)
{
	inout.final_color->set_min_filter(filter_linear);
	inout.final_color->set_mag_filter(filter_linear);

	ScopeTimeFunction();
	gc->set_frame_buffer(inout.fb_bloom_extract);
	gc->set_viewport(inout.bloom_contribution->size(), gc->texture_image_y_axis());
	gc->set_texture(0, inout.final_color);
	gc->set_blend_state(blend_state);
	gc->set_program_object(bloom_shader);
	gc->draw_primitives(type_triangles, 6, rect_primarray);
	gc->reset_program_object();
	gc->reset_texture(0);
	gc->reset_frame_buffer();

	inout.blur.input = inout.bloom_contribution;
	inout.blur.output = inout.fb_bloom_extract;
	inout.blur.blur(gc, tf_rgba8, 4.0f, 15);
}
