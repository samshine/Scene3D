
#include "precomp.h"
#include "z_minmax.h"
#include "Scene3D/SceneCache/shader_setup.h"

using namespace uicore;

ZMinMax::ZMinMax(int tile_size)
: tile_size(tile_size), iterations(find_iterations(tile_size))
{
}

void ZMinMax::minmax(const GraphicContextPtr &gc)
{
	update_buffers(gc);

	normal_z.get()->set_min_filter(filter_nearest);
	normal_z.get()->set_mag_filter(filter_nearest);
	normal_z.get()->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);

	gc->set_primitives_array(prim_array);
	gc->set_program_object(program0);
	gc->set_blend_state(blend_state);

	Size texture_size = find_texture_size(normal_z.get());

	for (int i = 0; i < iterations; i++)
	{
		if (i == 1)
			gc->set_program_object(program1);

		int iteration_width = texture_size.width >> i;
		int iteration_height = texture_size.height >> i;

		gc->set_frame_buffer((i % 2 == 0) ? fb0 : fb1);
		gc->set_viewport(Size(iteration_width, iteration_height), gc->texture_image_y_axis());
		if (i == 0)
			gc->set_texture(0, normal_z.get());
		else
			gc->set_texture(0, (i % 2 == 0) ? texture1 : texture0);
		gc->draw_primitives_array(type_triangles, 6);
	}

	gc->reset_texture(0);
	gc->reset_program_object();
	gc->reset_primitives_array();
	gc->reset_frame_buffer();
	gc->set_viewport(viewport->get_size(), gc->texture_image_y_axis());
}

void ZMinMax::update_buffers(const GraphicContextPtr &gc)
{
	if (normal_z.updated())
	{
		Size texture_size = find_texture_size(normal_z.get());

		if (!texture0 || texture0->size() != texture_size)
		{ 
			std::string vertex_shader, fragment_shader0, fragment_shader1;
			if (gc->shader_language() == shader_glsl)
				get_shader_glsl(vertex_shader, fragment_shader0, fragment_shader1);
			else
				get_shader_hlsl(vertex_shader, fragment_shader0, fragment_shader1);

			program0 = compile_and_link(gc, vertex_shader, fragment_shader0);
			program1 = compile_and_link(gc, vertex_shader, fragment_shader1);

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

			texture0 = Texture2D::create(gc, texture_size.width, texture_size.height, tf_rg32f);
			texture0->set_min_filter(filter_nearest);
			texture0->set_mag_filter(filter_nearest);
			texture0->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);

			texture1 = Texture2D::create(gc, texture_size.width, texture_size.height, tf_rg32f);
			texture1->set_min_filter(filter_nearest);
			texture1->set_mag_filter(filter_nearest);
			texture1->set_wrap_mode(wrap_clamp_to_edge, wrap_clamp_to_edge);

			fb0 = FrameBuffer::create(gc);
			fb0->attach_color(0, texture0);

			fb1 = FrameBuffer::create(gc);
			fb1->attach_color(0, texture1);

			BlendStateDescription blend_desc;
			blend_desc.enable_blending(false);
			blend_state = gc->create_blend_state(blend_desc);
		}

		result.set((iterations % 2 == 0) ? texture1 : texture0);
	}
}

Size ZMinMax::find_texture_size(const Texture2DPtr &normal_z)
{
	Size framebuffer_size = normal_z->size();
	Size tile_dimensions = (framebuffer_size + (tile_size - 1)) / tile_size;
	return tile_dimensions * tile_size / 2;
}

ProgramObjectPtr ZMinMax::compile_and_link(const GraphicContextPtr &gc, const std::string &vertex_source, const std::string &fragment_source)
{
	auto vertex_shader = ShaderObject::create(gc, ShaderType::vertex, vertex_source);
	auto fragment_shader = ShaderObject::create(gc, ShaderType::fragment, fragment_source);
	if (!vertex_shader->try_compile())
		throw Exception(string_format("ZMinMax vertex program could not compile: %1", vertex_shader->info_log()));
	if (!fragment_shader->try_compile())
		throw Exception(string_format("ZMinMax fragment program could not compile: %1", fragment_shader->info_log()));

	auto program = ProgramObject::create(gc);
	program->attach(vertex_shader);
	program->attach(fragment_shader);
	program->bind_attribute_location(0, "PositionInProjection");
	program->bind_frag_data_location(0, "FragMinMax");
	ShaderSetup::link(program, "zminmax program");

	program->set_uniform1i("Texture", 0);
	return program;
}

int ZMinMax::find_iterations(int size)
{
	int iterations = 0;
	while (size > 1)
	{
		size >>= 1;
		iterations++;
	}
	return iterations;
}

void ZMinMax::get_shader_glsl(std::string &out_vertex_shader, std::string &out_fragment_shader0, std::string &out_fragment_shader1)
{
	out_vertex_shader =
		"#version 330\r\n"
		"in vec4 PositionInProjection;\r\n"
		"void main()\r\n"
		"{\r\n"
		"	gl_Position = PositionInProjection;\r\n"
		"}\r\n";

	out_fragment_shader0 =
		"#version 330\r\n"
		"out vec4 FragMinMax;\r\n"
		"uniform sampler2D Texture;\r\n"
		"void main()\r\n"
		"{\r\n"
		"	ivec2 size = textureSize(Texture, 0) - 1;\r\n"
		"	ivec2 pos = ivec2(gl_FragCoord.xy) * 2;\r\n"
		"	float z0 = texelFetch(Texture, min(pos, size), 0).w; \r\n"
		"	float z1 = texelFetch(Texture, min(pos + ivec2(1,0), size), 0).w; \r\n"
		"	float z2 = texelFetch(Texture, min(pos + ivec2(0,1), size), 0).w; \r\n"
		"	float z3 = texelFetch(Texture, min(pos + ivec2(1,1), size), 0).w; \r\n"
		"	vec2 zminmax = vec2(\r\n"
		"		min(min(min(z0, z1), z2), z3),\r\n"
		"		max(max(max(z0, z1), z2), z3));\r\n"
		"	FragMinMax = vec4(zminmax, 0, 0);\r\n"
		"}\r\n";

	out_fragment_shader1 =
		"#version 330\r\n"
		"out vec4 FragMinMax;\r\n"
		"uniform sampler2D Texture;\r\n"
		"void main()\r\n"
		"{\r\n"
		"	ivec2 pos = ivec2(gl_FragCoord.xy) * 2;\r\n"
		"	vec2 z0 = texelFetch(Texture, pos, 0).xy; \r\n"
		"	vec2 z1 = texelFetch(Texture, pos + ivec2(1,0), 0).xy; \r\n"
		"	vec2 z2 = texelFetch(Texture, pos + ivec2(0,1), 0).xy; \r\n"
		"	vec2 z3 = texelFetch(Texture, pos + ivec2(1,1), 0).xy; \r\n"
		"	vec2 zminmax = vec2(\r\n"
		"		min(min(min(z0.x, z1.x), z2.x), z3.x),\r\n"
		"		max(max(max(z0.y, z1.y), z2.y), z3.y));\r\n"
		"	FragMinMax = vec4(zminmax, 0, 0);\r\n"
		"}\r\n";
}

void ZMinMax::get_shader_hlsl(std::string &out_vertex_shader, std::string &out_fragment_shader0, std::string &out_fragment_shader1)
{
	out_vertex_shader =
		"struct VertexIn\r\n"
		"{\r\n"
		"	float4 PositionInProjection : PositionInProjection;\r\n"
		"};\r\n"
		"\r\n"
		"struct VertexOut\r\n"
		"{\r\n"
		"	float4 Position : SV_Position;\r\n"
		"};\r\n"
		"\r\n"
		"VertexOut main(VertexIn input)\r\n"
		"{\r\n"
		"	VertexOut output;\r\n"
		"	output.Position = input.PositionInProjection;\r\n"
		"	return output;\r\n"
		"}";

	out_fragment_shader0 =
		"struct PixelIn\r\n"
		"{\r\n"
		"	float4 ScreenPos : SV_Position;\r\n"
		"};\r\n"
		"\r\n"
		"struct PixelOut\r\n"
		"{\r\n"
		"	float4 FragMinMax : SV_Target0;\r\n"
		"};\r\n"
		"\r\n"
		"Texture2D Texture;\r\n"
		"\r\n"
		"PixelOut main(PixelIn input)\r\n"
		"{\r\n"
		"	PixelOut output;\r\n"
		"	uint width, height;\r\n"
		"	Texture.GetDimensions(width, height);\r\n"
		"	int2 size = int2(width, height) - 1;\r\n"
		"	int2 pos = int2(input.ScreenPos.xy) * 2;\r\n"
		"	float z0 = Texture.Load(int3(min(pos, size), 0)).w; \r\n"
		"	float z1 = Texture.Load(int3(min(pos + int2(1,0), size), 0)).w; \r\n"
		"	float z2 = Texture.Load(int3(min(pos + int2(0,1), size), 0)).w; \r\n"
		"	float z3 = Texture.Load(int3(min(pos + int2(1,1), size), 0)).w; \r\n"
		"	float2 zminmax = float2(\r\n"
		"		min(min(min(z0, z1), z2), z3),\r\n"
		"		max(max(max(z0, z1), z2), z3));\r\n"
		"	output.FragMinMax = float4(zminmax, 0, 0);\r\n"
		"	return output;\r\n"
		"}";

	out_fragment_shader1 =
		"struct PixelIn\r\n"
		"{\r\n"
		"	float4 ScreenPos : SV_Position;\r\n"
		"};\r\n"
		"\r\n"
		"struct PixelOut\r\n"
		"{\r\n"
		"	float4 FragMinMax : SV_Target0;\r\n"
		"};\r\n"
		"\r\n"
		"Texture2D Texture;\r\n"
		"\r\n"
		"PixelOut main(PixelIn input)\r\n"
		"{\r\n"
		"	PixelOut output;\r\n"
		"	int2 pos = int2(input.ScreenPos.xy) * 2;\r\n"
		"	float2 z0 = Texture.Load(int3(pos, 0)).xy; \r\n"
		"	float2 z1 = Texture.Load(int3(pos + int2(1,0), 0)).xy; \r\n"
		"	float2 z2 = Texture.Load(int3(pos + int2(0,1), 0)).xy; \r\n"
		"	float2 z3 = Texture.Load(int3(pos + int2(1,1), 0)).xy; \r\n"
		"	float2 zminmax = float2(\r\n"
		"		min(min(min(z0.x, z1.x), z2.x), z3.x),\r\n"
		"		max(max(max(z0.y, z1.y), z2.y), z3.y));\r\n"
		"	output.FragMinMax = float4(zminmax, 0, 0);\r\n"
		"	return output;\r\n"
		"}";
}
