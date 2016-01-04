
#include "precomp.h"
#include "gaussian_blur.h"
#include "Scene3D/Performance/scope_timer.h"

using namespace uicore;

GaussianBlur::GaussianBlur()
{
}

void GaussianBlur::vertical(const uicore::GraphicContextPtr &gc, float blur_amount, int sample_count, uicore::Texture2DPtr input, uicore::FrameBufferPtr output)
{
	ScopeTimeFunction();

	setup(gc, blur_amount, sample_count);

	input.get()->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);
	input.get()->set_mag_filter(filter_nearest);
	input.get()->set_min_filter(filter_nearest);

	auto size = output->get_size();

	gc->set_frame_buffer(output);
	gc->set_viewport(Rectf(0.0f, 0.0f, (float)size.width, (float)size.height), gc->texture_image_y_axis());
	gc->set_blend_state(blend_state);
	gc->set_program_object(blur_setups[blur_setup_index].vertical_blur_program);
	gc->set_texture(0, input);
	gc->draw_primitives(type_triangles, 6, prim_array);
	gc->set_texture(0, nullptr);
	gc->set_program_object(nullptr);
	gc->set_blend_state(nullptr);
	gc->set_frame_buffer(nullptr);
	gc->set_viewport(Rectf(0.0f, 0.0f, (float)gc->width(), (float)gc->height()), gc->texture_image_y_axis());
}

void GaussianBlur::horizontal(const uicore::GraphicContextPtr &gc, float blur_amount, int sample_count, uicore::Texture2DPtr input, uicore::FrameBufferPtr output)
{
	ScopeTimeFunction();

	setup(gc, blur_amount, sample_count);

	input.get()->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);
	input.get()->set_mag_filter(filter_nearest);
	input.get()->set_min_filter(filter_nearest);

	auto size = output->get_size();

	gc->set_frame_buffer(output);
	gc->set_viewport(Rectf(0.0f, 0.0f, (float)size.width, (float)size.height), gc->texture_image_y_axis());
	gc->set_blend_state(blend_state);
	gc->set_program_object(blur_setups[blur_setup_index].horizontal_blur_program);
	gc->set_texture(0, input);
	gc->draw_primitives(type_triangles, 6, prim_array);
	gc->set_texture(0, nullptr);
	gc->set_program_object(nullptr);
	gc->set_blend_state(nullptr);
	gc->set_frame_buffer(nullptr);
	gc->set_viewport(Rectf(0.0f, 0.0f, (float)gc->width(), (float)gc->height()), gc->texture_image_y_axis());
}

void GaussianBlur::setup(const GraphicContextPtr &gc, float blur_amount, int sample_count)
{
	for (blur_setup_index = 0; blur_setup_index < blur_setups.size(); blur_setup_index++)
	{
		if (blur_setups[blur_setup_index].blur_amount == blur_amount && blur_setups[blur_setup_index].sample_count == sample_count)
			return;
	}

	std::string vertex_shader, vertical_fragment_shader, horizontal_fragment_shader;
	if (gc->shader_language() == shader_glsl)
	{
		vertex_shader = vertex_shader_glsl();
		horizontal_fragment_shader = fragment_shader_glsl(blur_amount, sample_count, false);
		vertical_fragment_shader = fragment_shader_glsl(blur_amount, sample_count, true);
	}
	else
	{
		vertex_shader = vertex_shader_hlsl();
		horizontal_fragment_shader = fragment_shader_hlsl(blur_amount, sample_count, false);
		vertical_fragment_shader = fragment_shader_hlsl(blur_amount, sample_count, true);
	}

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
	blur_setup.vertical_blur_program->set_uniform1i("SourceSampler", 0);

	blur_setup.horizontal_blur_program = ProgramObject::create(gc);
	blur_setup.horizontal_blur_program->attach(vertex);
	blur_setup.horizontal_blur_program->attach(horizontal_fragment);
	blur_setup.horizontal_blur_program->bind_frag_data_location(0, "FragColor");
	blur_setup.horizontal_blur_program->bind_attribute_location(0, "PositionInProjection");
	if (!blur_setup.horizontal_blur_program->try_link())
		throw Exception("Could not link horizontal Gaussian blur program");
	blur_setup.horizontal_blur_program->set_uniform1i("SourceTexture", 0);
	blur_setup.horizontal_blur_program->set_uniform1i("SourceSampler", 0);

	blur_setups.push_back(blur_setup);

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

std::string GaussianBlur::vertex_shader_glsl()
{
	return R"(
		#version 330

		in vec4 PositionInProjection;
		out vec2 TexCoord;

		void main()
		{
			gl_Position = PositionInProjection;
			TexCoord = (gl_Position.xy + 1.0) * 0.5;
		}
	)";
}

std::string GaussianBlur::vertex_shader_hlsl()
{
	return R"(
		struct VertexIn
		{
			float4 PositionInProjection : PositionInProjection;
		};
		
		struct VertexOut
		{
			float4 Position : SV_Position;
			float2 TexCoord : TexCoord;
		};
		
		VertexOut main(VertexIn input)
		{
			VertexOut output;
			output.Position = input.PositionInProjection;
			output.TexCoord = output.Position.xy * 0.5 + 0.5;
			return output;
		}
	)";
}

std::string GaussianBlur::fragment_shader_glsl(float blur_amount, int sample_count, bool vertical)
{
	std::vector<float> sample_weights;
	std::vector<int> sample_offsets;
	compute_blur_samples(sample_count, blur_amount, sample_weights, sample_offsets);

	std::string fragment_shader =
		R"(
		#version 330
		in vec2 TexCoord;
		uniform sampler2D SourceTexture;
		out vec4 FragColor;
		void main()
		{
			FragColor = %1;
		}
		)";

	std::string loop_code;
	for (int i = 0; i < sample_count; i++)
	{
		if (i > 0)
			loop_code += " + ";

		if (vertical)
			loop_code += string_format("\r\n\t\t\ttextureOffset(SourceTexture, TexCoord, ivec2(0, %1)) * %2", sample_offsets[i], sample_weights[i]);
		else
			loop_code += string_format("\r\n\t\t\ttextureOffset(SourceTexture, TexCoord, ivec2(%1, 0)) * %2", sample_offsets[i], sample_weights[i]);
	}

	return string_format(fragment_shader, loop_code);
}

std::string GaussianBlur::fragment_shader_hlsl(float blur_amount, int sample_count, bool vertical)
{
	std::vector<float> sample_weights;
	std::vector<int> sample_offsets;
	compute_blur_samples(sample_count, blur_amount, sample_weights, sample_offsets);

	std::string fragment_shader =
		R"(
		struct PixelIn
		{
			float4 ScreenPos : SV_Position;
			float2 TexCoord : TexCoord;
		};
		
		struct PixelOut
		{
			float4 FragColor : SV_Target0;
		};
		
		Texture2D SourceTexture;
		SamplerState SourceSampler;
		
		PixelOut main(PixelIn input)
		{
			PixelOut output;
			output.FragColor = %1;
			return output;
		}
		)";

	std::string loop_code;
	for (int i = 0; i < sample_count; i++)
	{
		if (i > 0)
			loop_code += " + ";

		if (vertical)
			loop_code += string_format("\r\n\t\t\tSourceTexture.Sample(SourceSampler, input.TexCoord, int2(0, %1)) * %2", sample_offsets[i], sample_weights[i]);
		else
			loop_code += string_format("\r\n\t\t\tSourceTexture.Sample(SourceSampler, input.TexCoord, int2(%1, 0)) * %2", sample_offsets[i], sample_weights[i]);
	}

	return string_format(fragment_shader, loop_code);
}

float GaussianBlur::compute_gaussian(float n, float theta) // theta = Blur Amount
{
	return (float)((1.0f / sqrtf(2 * (float)PI * theta)) * expf(-(n * n) / (2.0f * theta * theta)));
}

void GaussianBlur::compute_blur_samples(int sample_count, float blur_amount, std::vector<float> &sample_weights, std::vector<int> &sample_offsets)
{
	sample_weights.resize(sample_count);
	sample_offsets.resize(sample_count);

	sample_weights[0] = compute_gaussian(0, blur_amount);
	sample_offsets[0] = 0;

	float total_weights = sample_weights[0];

	for (int i = 0; i < sample_count / 2; i++)
	{
		float weight = compute_gaussian(i + 1.0f, blur_amount);

		sample_weights[i * 2 + 1] = weight;
		sample_weights[i * 2 + 2] = weight;
		sample_offsets[i * 2 + 1] = i + 1;
		sample_offsets[i * 2 + 2] = i - 1;

		total_weights += weight * 2;
	}

	for (int i = 0; i < sample_count; i++)
	{
		sample_weights[i] /= total_weights;
	}
}
