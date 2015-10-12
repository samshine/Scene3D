
#include "precomp.h"
#include "bloom_pass.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneCache/shader_setup.h"

using namespace uicore;

BloomPass::BloomPass(const GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout)
{
	viewport = inout.get<Rect>("Viewport");
	final_color = inout.get<Texture2DPtr>("FinalColor");
	bloom_contribution = inout.get<Texture2DPtr>("BloomContribution");

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

	bloom_blur.input = bloom_contribution;

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	blend_state = gc->create_blend_state(blend_desc);
}

void BloomPass::run(const GraphicContextPtr &gc)
{
	final_color.get()->set_min_filter(filter_linear);
	final_color.get()->set_mag_filter(filter_linear);

	ScopeTimeFunction();
	setup_bloom_extract(gc);
	gc->set_frame_buffer(fb_bloom_extract);
	gc->set_viewport(bloom_contribution.get()->size(), gc->texture_image_y_axis());
	gc->set_texture(0, final_color.get());
	gc->set_blend_state(blend_state);
	gc->set_program_object(bloom_shader);
	gc->draw_primitives(type_triangles, 6, rect_primarray);
	gc->reset_program_object();
	gc->reset_texture(0);
	gc->reset_frame_buffer();

	bloom_blur.blur(gc, tf_rgba8, 4.0f, 15);
}

void BloomPass::setup_bloom_extract(const GraphicContextPtr &gc)
{
	Size viewport_size = viewport->size();
	Size bloom_size = viewport_size / 2;
	if (!bloom_contribution.get() || bloom_contribution.get()->size() != bloom_size || !gc->is_frame_buffer_owner(fb_bloom_extract))
	{
		bloom_contribution.set(nullptr);
		fb_bloom_extract.reset();
		gc->flush();

		bloom_contribution.set(Texture2D::create(gc, bloom_size.width, bloom_size.height, tf_rgba8));

		fb_bloom_extract = FrameBuffer::create(gc);
		fb_bloom_extract->attach_color(0, bloom_contribution.get());

		bloom_blur.output.set(fb_bloom_extract);
	}
}
