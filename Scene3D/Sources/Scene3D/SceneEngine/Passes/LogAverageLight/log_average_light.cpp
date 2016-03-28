
#include "precomp.h"
#include "log_average_light.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "vertex_log_average_glsl.h"
#include "fragment_log_average_glsl.h"
#include "fragment_log_downscale_glsl.h"
#include "fragment_exposure_glsl.h"

using namespace uicore;

LogAverageLight::LogAverageLight(const GraphicContextPtr &gc, SceneRender &inout, int iterations)
: inout(inout), iterations(iterations), current_index(0)
{
	Size texture_size(1 << iterations, 1 << iterations);

	std::string vertex_program = vertex_log_average_glsl();
	std::string fragment_program0 = string_format(fragment_log_average_glsl(), texture_size.width);

	std::string fragment_program1 = fragment_log_downscale_glsl();
	std::string fragment_program2 = fragment_exposure_glsl();

	program0 = compile_and_link(gc, vertex_program, fragment_program0);
	program1 = compile_and_link(gc, vertex_program, fragment_program1);
	program2 = compile_and_link(gc, vertex_program, fragment_program2);

	Vec4f positions[6] =
	{
		Vec4f(-1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f( 1.0f,  1.0f, 1.0f, 1.0f)
	};
	vertices = VertexArrayVector<Vec4f>(gc, positions, 6);

	prim_array = PrimitivesArray::create(gc);
	prim_array->set_attributes(0, vertices);

	texture0 = Texture2D::create(gc, texture_size.width, texture_size.height, tf_r32f);
	texture0->set_min_filter(filter_nearest);
	texture0->set_mag_filter(filter_nearest);
	texture0->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);

	texture1 = Texture2D::create(gc, texture_size.width, texture_size.height, tf_r32f);
	texture1->set_min_filter(filter_nearest);
	texture1->set_mag_filter(filter_nearest);
	texture1->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);

	auto black = PixelBuffer::create(1, 1, tf_r32f);
	black->data<float>()[0] = 0.0f;

	result_texture0 = Texture2D::create(gc, 1, 1, tf_r32f);
	result_texture0->set_min_filter(filter_nearest);
	result_texture0->set_mag_filter(filter_nearest);
	result_texture0->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);
	result_texture0->set_subimage(gc, 0, 0, black, black->size());

	result_texture1 = Texture2D::create(gc, 1, 1, tf_r32f);
	result_texture1->set_min_filter(filter_nearest);
	result_texture1->set_mag_filter(filter_nearest);
	result_texture1->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);
	result_texture1->set_subimage(gc, 0, 0, black, black->size());

	fb0 = FrameBuffer::create(gc);
	fb0->attach_color(0, texture0);

	fb1 = FrameBuffer::create(gc);
	fb1->attach_color(0, texture1);

	fb2 = FrameBuffer::create(gc);
	fb2->attach_color(0, result_texture0);

	fb3 = FrameBuffer::create(gc);
	fb3->attach_color(0, result_texture1);

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	blend_state = gc->create_blend_state(blend_desc);
}

Texture2DPtr &LogAverageLight::find_log_average_light(const GraphicContextPtr &gc, Texture2DPtr &hdr_texture)
{
	hdr_texture->set_min_filter(filter_nearest);
	hdr_texture->set_mag_filter(filter_nearest);
	hdr_texture->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);

	gc->set_primitives_array(prim_array);
	gc->set_program_object(program0);
	gc->set_blend_state(blend_state);

	Size texture_size(1 << iterations, 1 << iterations);

	for (int i = 0; i <= iterations; i++)
	{
		if (i == 1)
			gc->set_program_object(program1);

		int iteration_width = texture_size.width >> i;
		int iteration_height = texture_size.height >> i;

		gc->set_frame_buffer((i % 2 == 0) ? fb0 : fb1);
		gc->set_viewport(Size(iteration_width, iteration_height), gc->texture_image_y_axis());
		if (i == 0)
			gc->set_texture(0, hdr_texture);
		else
			gc->set_texture(0, (i % 2 == 0) ? texture1 : texture0);
		gc->draw_primitives_array(type_triangles, 6);
	}

	current_index = (current_index + 1) % 2;

	gc->set_program_object(program2);
	gc->set_frame_buffer(current_index == 0 ? fb2 : fb3);
	gc->set_viewport(Rectf(0.0f, 0.0f, 1.0f, 1.0f), gc->texture_image_y_axis());
	gc->set_texture(0, (iterations % 2 == 0) ? texture1 : texture0);
	gc->set_texture(1, current_index == 0 ? result_texture1 : result_texture0);
	gc->draw_primitives_array(type_triangles, 6);

	gc->reset_texture(0);
	gc->reset_texture(1);
	gc->reset_program_object();
	gc->reset_primitives_array();
	gc->reset_frame_buffer();
	gc->set_viewport(inout.viewport_size, gc->texture_image_y_axis());

	return current_index == 0 ? result_texture0 : result_texture1;
}

ProgramObjectPtr LogAverageLight::compile_and_link(const GraphicContextPtr &gc, const std::string &vertex_source, const std::string &fragment_source)
{
	auto vertex_shader = ShaderObject::create(gc, ShaderType::vertex, vertex_source);
	auto fragment_shader = ShaderObject::create(gc, ShaderType::fragment, fragment_source);
	if (!vertex_shader->try_compile())
		throw Exception(string_format("LogAverageLight vertex program could not compile: %1", vertex_shader->info_log()));
	if (!fragment_shader->try_compile())
		throw Exception(string_format("LogAverageLight fragment program could not compile: %1", fragment_shader->info_log()));

	auto program = ProgramObject::create(gc);
	program->attach(vertex_shader);
	program->attach(fragment_shader);
	program->bind_attribute_location(0, "PositionInProjection");
	program->bind_frag_data_location(0, "FragAverage");

	ShaderSetup::link(program, "log average light program");

	program->set_uniform1i("Texture", 0);
	program->set_uniform1i("TextureCurrent", 0);
	program->set_uniform1i("TexturePrev", 1);
	return program;
}
