
#include "precomp.h"
#include "gaussian_blur.h"

using namespace uicore;

GaussianBlur::GaussianBlur()
: format(tf_rgba8)
{
}

void GaussianBlur::blur(const GraphicContextPtr &gc, TextureFormat format, float blur_amount, int sample_count)
{
	setup(gc, input.get()->size(), format, blur_amount, sample_count);

	input.get()->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);
	input.get()->set_mag_filter(filter_nearest);
	input.get()->set_min_filter(filter_nearest);

	gc->set_blend_state(blend_state);

	// Horizontal blur
	gc->set_program_object(current_blur_setup->horizontal_blur_program);
	gc->set_frame_buffer(fb0);
	gc->set_viewport(Rectf(0.0f, 0.0f, (float)size.width, (float)size.height), gc->texture_image_y_axis());
	gc->set_texture(0, input.get());
	gc->draw_primitives(type_triangles, 6, prim_array);
	gc->reset_texture(0);
	gc->reset_frame_buffer();

	// Vertical blur
	gc->set_program_object(current_blur_setup->vertical_blur_program);
	gc->set_frame_buffer(output.get());
	gc->set_viewport(Rectf(0.0f, 0.0f, (float)size.width, (float)size.height), gc->texture_image_y_axis());
	gc->set_texture(0, pass0_texture);
	gc->draw_primitives(type_triangles, 6, prim_array);
	gc->reset_texture(0);
	gc->reset_frame_buffer();

	gc->reset_program_object();
}

void GaussianBlur::setup(const GraphicContextPtr &gc, Size new_size, TextureFormat new_format, float blur_amount, int sample_count)
{
	size = new_size;

	for (current_blur_setup = blur_setups.begin(); current_blur_setup != blur_setups.end(); ++current_blur_setup)
	{
		if (current_blur_setup->blur_amount == blur_amount && current_blur_setup->sample_count == sample_count)
			break;
	}

	if (current_blur_setup == blur_setups.end())
	{
		std::string vertex_shader, vertical_fragment_shader, horizontal_fragment_shader;
		if (gc->shader_language() == shader_glsl)
			get_shader_glsl(blur_amount, sample_count, vertex_shader, vertical_fragment_shader, horizontal_fragment_shader);
		else
			get_shader_hlsl(blur_amount, sample_count, vertex_shader, vertical_fragment_shader, horizontal_fragment_shader);

		auto vertex = ShaderObject::create(gc, ShaderType::vertex, vertex_shader);
		auto vertical_fragment = ShaderObject::create(gc, ShaderType::fragment, vertical_fragment_shader);
		auto horizontal_fragment = ShaderObject::create(gc, ShaderType::fragment, horizontal_fragment_shader);
		if (!vertex->try_compile())
			throw Exception(string_format("Could not compile Gaussian blur vertex shader: %1", vertex->info_log()));
		if (!vertical_fragment->try_compile())
			throw Exception(string_format("Could not compile vertical Gaussian blur fragment shader: %1", vertical_fragment->info_log()));
		if (!horizontal_fragment->try_compile())
			throw Exception(string_format("Could not compile horizontal Gaussian blur fragment shader: %1", horizontal_fragment->info_log()));

		BlurSetup blur_setup(blur_amount, sample_count);
		blur_setup.vertical_blur_program = ProgramObject::create(gc);
		blur_setup.vertical_blur_program->attach(vertex);
		blur_setup.vertical_blur_program->attach(vertical_fragment);
		blur_setup.vertical_blur_program->bind_frag_data_location(0, "FragColor");
		blur_setup.vertical_blur_program->bind_attribute_location(0, "PositionInProjection");
		if (!blur_setup.vertical_blur_program->try_link())
			throw Exception("Could not link vertical Gaussian blur program");
		blur_setup.vertical_blur_program->set_uniform1i("SourceTexture", 0);

		blur_setup.horizontal_blur_program = ProgramObject::create(gc);
		blur_setup.horizontal_blur_program->attach(vertex);
		blur_setup.horizontal_blur_program->attach(horizontal_fragment);
		blur_setup.horizontal_blur_program->bind_frag_data_location(0, "FragColor");
		blur_setup.horizontal_blur_program->bind_attribute_location(0, "PositionInProjection");
		if (!blur_setup.horizontal_blur_program->try_link())
			throw Exception("Could not link horizontal Gaussian blur program");
		blur_setup.horizontal_blur_program->set_uniform1i("SourceTexture", 0);

		blur_setups.push_back(blur_setup);
		current_blur_setup = blur_setups.begin() + (blur_setups.size() - 1);
	}

	if ((pass0_texture && (pass0_texture->width() != size.width || pass0_texture->height() != size.height)) || format != new_format)
	{
		pass0_texture.reset();
		fb0.reset();
		format = new_format;
	}

	if (!pass0_texture)
	{
		pass0_texture = Texture2D::create(gc, size.width, size.height, format);
		pass0_texture->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);
		pass0_texture->set_mag_filter(filter_nearest);
		pass0_texture->set_min_filter(filter_nearest);
	}

	if (!fb0 || !gc->is_frame_buffer_owner(fb0))
	{
		fb0 = FrameBuffer::create(gc);
		fb0->attach_color(0, pass0_texture);
	}

	if (!gpu_positions.buffer())
	{
		Vec4f positions[6] =
		{
			Vec4f(-1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
			Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
			Vec4f( 1.0f,  1.0f, 1.0f, 1.0f)
		};
		gpu_positions = VertexArrayVector<Vec4f>(gc, positions, 6);

		prim_array = PrimitivesArray::create(gc);
		prim_array->set_attributes(0, gpu_positions);

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(false);
		blend_state = gc->create_blend_state(blend_desc);
	}
}

void GaussianBlur::get_shader_glsl(float blur_amount, int sample_count, std::string &out_vertex_shader, std::string &out_vertical_fragment_shader, std::string &out_horizontal_fragment_shader)
{
	std::vector<float> sample_weights_x, sample_weights_y;
	std::vector<Vec2i> sample_offsets_x, sample_offsets_y;
	compute_blur_samples(sample_count, blur_amount, 1, 0, sample_weights_x, sample_offsets_x);
	compute_blur_samples(sample_count, blur_amount, 0, 1, sample_weights_y, sample_offsets_y);

	out_vertex_shader =
		"#version 330\r\n"
		"\r\n"
		"in vec4 PositionInProjection;\r\n"
		"out vec2 TexCoord;\r\n"
		"\r\n"
		"void main()\r\n"
		"{\r\n"
		"	gl_Position = PositionInProjection;\r\n"
		"	TexCoord = (gl_Position.xy + 1.0) * 0.5;\r\n"
		"}";

	std::string fragment_shader =
		"#version 330\r\n"
		"\r\n"
		"in vec2 TexCoord;\r\n"
		"\r\n"
		"uniform sampler2D SourceTexture;\r\n"
		"out vec4 FragColor;\r\n"
		"\r\n"
		"void main()\r\n"
		"{\r\n"
		"	FragColor = %1;\r\n"
		"}";

	std::string vertical_loop_code;
	std::string horizontal_loop_code;
	for (int i = 0; i < sample_count; i++)
	{
		if (i > 0)
		{
			vertical_loop_code += " + ";
			horizontal_loop_code += " + ";
		}
		horizontal_loop_code += string_format("\r\n\t\ttextureOffset(SourceTexture, TexCoord, ivec2(%1,%2)) * %3", sample_offsets_x[i].x, sample_offsets_x[i].y, sample_weights_x[i]);
		vertical_loop_code   += string_format("\r\n\t\ttextureOffset(SourceTexture, TexCoord, ivec2(%1,%2)) * %3", sample_offsets_y[i].x, sample_offsets_y[i].y, sample_weights_y[i]);
	}

	out_vertical_fragment_shader = string_format(fragment_shader, vertical_loop_code);
	out_horizontal_fragment_shader = string_format(fragment_shader, horizontal_loop_code);
}

void GaussianBlur::get_shader_hlsl(float blur_amount, int sample_count, std::string &out_vertex_shader, std::string &out_vertical_fragment_shader, std::string &out_horizontal_fragment_shader)
{
	std::vector<float> sample_weights_x, sample_weights_y;
	std::vector<Vec2i> sample_offsets_x, sample_offsets_y;
	compute_blur_samples(sample_count, blur_amount, 1, 0, sample_weights_x, sample_offsets_x);
	compute_blur_samples(sample_count, blur_amount, 0, 1, sample_weights_y, sample_offsets_y);

	out_vertex_shader =
		"struct VertexIn\r\n"
		"{\r\n"
		"	float4 PositionInProjection : PositionInProjection;\r\n"
		"};\r\n"
		"\r\n"
		"struct VertexOut\r\n"
		"{\r\n"
		"	float4 Position : SV_Position;\r\n"
		"	float2 TexCoord : TexCoord;\r\n"
		"};\r\n"
		"\r\n"
		"VertexOut main(VertexIn input)\r\n"
		"{\r\n"
		"	VertexOut output;\r\n"
		"	output.Position = input.PositionInProjection;\r\n"
		"	output.TexCoord = output.Position.xy * 0.5 + 0.5;\r\n"
		"	return output;\r\n"
		"}";

	std::string fragment_shader =
		"struct PixelIn\r\n"
		"{\r\n"
		"	float4 ScreenPos : SV_Position;\r\n"
		"	float2 TexCoord : TexCoord;\r\n"
		"};\r\n"
		"\r\n"
		"struct PixelOut\r\n"
		"{\r\n"
		"	float4 FragColor : SV_Target0;\r\n"
		"};\r\n"
		"\r\n"
		"Texture2DPtr SourceTexture;\r\n"
		"SamplerState SourceSampler;\r\n"
		"\r\n"
		"PixelOut main(PixelIn input)\r\n"
		"{\r\n"
		"	PixelOut output;\r\n"
		"	output.FragColor = %1;\r\n"
		"	return output;\r\n"
		"}";

	std::string vertical_loop_code;
	std::string horizontal_loop_code;
	for (int i = 0; i < sample_count; i++)
	{
		if (i > 0)
		{
			vertical_loop_code += " + ";
			horizontal_loop_code += " + ";
		}
		horizontal_loop_code += string_format("\r\n\t\tSourceTexture.Sample(SourceSampler, input.TexCoord, int2(%1,%2)) * %3", sample_offsets_x[i].x, sample_offsets_x[i].y, sample_weights_x[i]);
		vertical_loop_code   += string_format("\r\n\t\tSourceTexture.Sample(SourceSampler, input.TexCoord, int2(%1,%2)) * %3", sample_offsets_y[i].x, sample_offsets_y[i].y, sample_weights_y[i]);
	}

	out_vertical_fragment_shader = string_format(fragment_shader, vertical_loop_code);
	out_horizontal_fragment_shader = string_format(fragment_shader, horizontal_loop_code);
}

float GaussianBlur::compute_gaussian(float n, float theta) // theta = Blur Amount
{
	return (float)((1.0f / sqrtf(2 * (float)PI * theta)) * expf(-(n * n) / (2.0f * theta * theta)));
}

void GaussianBlur::compute_blur_samples(int sample_count, float blur_amount, int dx, int dy, std::vector<float> &sample_weights, std::vector<Vec2i> &sample_offsets)
{
	sample_weights.resize(sample_count);
	sample_offsets.resize(sample_count);
	sample_weights[0] = compute_gaussian(0, blur_amount);
	sample_offsets[0] = Vec2i(0, 0);
	float total_weights = sample_weights[0];
	for (int i = 0; i < sample_count / 2; i++)
	{
		float weight = compute_gaussian(i + 1.0f, blur_amount);
		sample_weights[i * 2 + 1] = weight;
		sample_weights[i * 2 + 2] = weight;
		total_weights += weight * 2;
		int sample_offset = i + 1;
		Vec2i delta = Vec2i(dx * sample_offset, dy * sample_offset);
		sample_offsets[i * 2 + 1] = delta;
		sample_offsets[i * 2 + 2] = Vec2i(-delta.x, -delta.y);
	}
	for (int i = 0; i < sample_count; i++)
	{
		sample_weights[i] /= total_weights;
	}
}
