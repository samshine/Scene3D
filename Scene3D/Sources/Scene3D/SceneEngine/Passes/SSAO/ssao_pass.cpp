
#include "precomp.h"
#include "ssao_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/Performance/scope_timer.h"
#include "vertex_ssao_extract_glsl.h"
#include "vertex_ssao_extract_hlsl.h"
#include "fragment_ssao_extract_glsl.h"
#include "fragment_ssao_extract_hlsl.h"

using namespace uicore;

SSAOPass::SSAOPass(const GraphicContextPtr &gc, SceneRender &inout) : inout(inout)
{
	if (gc->shader_language() == shader_glsl)
	{
		extract_shader = ShaderSetup::compile(gc, "ssao", vertex_ssao_extract_glsl(), fragment_ssao_extract_glsl(), "");
		extract_shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		extract_shader = ShaderSetup::compile(gc, "ssao", vertex_ssao_extract_hlsl(), fragment_ssao_extract_hlsl(), "");
	}

	extract_shader->bind_attribute_location(0, "PositionInProjection");
	extract_shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(extract_shader, "ssao extract program");

	extract_shader->set_uniform_buffer_index("Uniforms", 0);
	extract_shader->set_uniform1i("NormalZ", 0);
	extract_shader->set_uniform1i("NormalZSampler", 0);

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

void SSAOPass::run()
{
	ScopeTimeFunction();

	if (!uniforms.buffer()) // To do: also do this if viewport size changes
	{
		UniformBuffer uniform_buffer;
		for (int i = 0; i < 160; i++)
		{
			Vec3f sample_vector;
			do
			{
				sample_vector = Vec3f(random_value(), random_value(), random_value());
			} while (sample_vector.length() < 0.1f);
			float sample_weight = (1.733f - sample_vector.length()) * 0.3f;
			uniform_buffer.sampledata[i] = Vec4f(sample_vector * 0.4f, sample_weight);
		}
		float aspect = inout.viewport.width() / (float)inout.viewport.height();
		float field_of_view_y_degrees = inout.field_of_view;
		float field_of_view_y_rad = (float)(field_of_view_y_degrees * PI / 180.0);
		float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
		uniform_buffer.f = f;
		uniform_buffer.f_div_aspect = f / aspect;
		uniforms = UniformVector<UniformBuffer>(inout.gc, &uniform_buffer, 1);
	}

	inout.normal_z_gbuffer->set_min_filter(filter_nearest);
	inout.normal_z_gbuffer->set_mag_filter(filter_nearest);

	inout.gc->set_frame_buffer(inout.fb_ambient_occlusion);
	inout.gc->set_viewport(inout.ambient_occlusion->size(), inout.gc->texture_image_y_axis());
	inout.gc->set_texture(0, inout.normal_z_gbuffer);
	inout.gc->set_uniform_buffer(0, uniforms);
	inout.gc->set_blend_state(blend_state);
	inout.gc->set_program_object(extract_shader);
	inout.gc->draw_primitives(type_triangles, 6, rect_primarray);
	inout.gc->reset_program_object();
	inout.gc->set_uniform_buffer(0, nullptr);
	inout.gc->set_blend_state(nullptr);
	inout.gc->reset_frame_buffer();

	//inout.blur.input = inout.ambient_occlusion;
	//inout.blur.output = inout.fb_ambient_occlusion;
	//inout.blur.blur(inout.gc, tf_r8, 3.0f, 15);
}

float SSAOPass::random_value()
{
	float v = rand() / (float)RAND_MAX;
	return v * 2.0f - 1.0f;
}
