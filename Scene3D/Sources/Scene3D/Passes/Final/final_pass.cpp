
#include "precomp.h"
#include "final_pass.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneCache/shader_setup.h"

using namespace uicore;

FinalPass::FinalPass(const GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout)
{
	viewport_fb = inout.get<FrameBufferPtr>("ViewportFrameBuffer");
	viewport = inout.get<Rect>("Viewport");
	final_color = inout.get<Texture2DPtr>("FinalColor");
	bloom_blur_texture = inout.get<Texture2DPtr>("BloomContribution");
	ambient_occlusion = inout.get<Texture2DPtr>("AmbientOcclusion");

	if (gc->shader_language() == shader_glsl)
	{
		present_shader = ShaderSetup::compile(gc, "", PathHelp::combine(shader_path, "Final/vertex_present.glsl"), PathHelp::combine(shader_path, "Final/fragment_present.glsl"), "");
		present_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		present_shader = ShaderSetup::compile(gc, "", PathHelp::combine(shader_path, "Final/vertex_present.hlsl"), PathHelp::combine(shader_path, "Final/fragment_present.hlsl"), "");
	}
	if (!present_shader->try_link())
		throw Exception("Shader linking failed!");
	present_shader->bind_attribute_location(0, "PositionInProjection");
	present_shader->set_uniform1i("FinalColors", 0);
	present_shader->set_uniform1i("FinalColorsSampler", 0);
	present_shader->set_uniform1i("LogAverageLight", 1);
	present_shader->set_uniform1i("BloomColors", 2);
	present_shader->set_uniform1i("BloomColorsSampler", 2);
	present_shader->set_uniform1i("AmbientOcclusion", 3);

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

	RasterizerStateDescription rasterizer_desc;
	rasterizer_desc.set_culled(false);
	rasterizer_state = gc->create_rasterizer_state(rasterizer_desc);
}

void FinalPass::run(const GraphicContextPtr &gc, SceneImpl *scene)
{
	ScopeTimeFunction();
	//Texture2DPtr &log_average_light_texture = log_average_light.find_log_average_light(gc, lightsource_pass.final_color);

	bloom_blur_texture.get()->set_min_filter(filter_linear);
	bloom_blur_texture.get()->set_mag_filter(filter_linear);
	final_color.get()->set_min_filter(filter_nearest);
	final_color.get()->set_mag_filter(filter_nearest);

	if (viewport_fb.get()) gc->set_frame_buffer(viewport_fb.get());
	gc->set_viewport(viewport.get(), gc->texture_image_y_axis());
	gc->set_texture(0, final_color.get());
	gc->set_texture(2, bloom_blur_texture.get());
	gc->set_program_object(present_shader);
	gc->set_rasterizer_state(rasterizer_state);
	gc->draw_primitives(type_triangles, 6, rect_primarray);
	gc->reset_program_object();
	gc->reset_texture(0);
	gc->reset_texture(2);
	gc->reset_frame_buffer();
}
