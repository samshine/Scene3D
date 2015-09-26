
#include "precomp.h"
#include "ssao_pass.h"
#include "Scene3D/SceneCache/shader_setup.h"

using namespace uicore;

SSAOPass::SSAOPass(GraphicContext &gc, const std::string &shader_path, ResourceContainer &inout)
{
	normal_z_gbuffer = inout.get<Texture2D>("NormalZGBuffer");

	ssao_contribution = inout.get<Texture2D>("AmbientOcclusion");

	if (gc.get_shader_language() == shader_glsl)
	{
		extract_shader = ShaderSetup::compile(gc, "", PathHelp::combine(shader_path, "Final/vertex_present.glsl"), PathHelp::combine(shader_path, "SSAO/fragment_ssao_extract.glsl"), "");
		extract_shader.bind_frag_data_location(0, "FragColor");
	}
	else
	{
		extract_shader = ShaderSetup::compile(gc, "", PathHelp::combine(shader_path, "Final/vertex_present.hlsl"), PathHelp::combine(shader_path, "SSAO/fragment_ssao_extract.hlsl"), "");
	}
	ShaderSetup::link(extract_shader, "ssao extract program");
	extract_shader.bind_attribute_location(0, "AttrPositionInProjection");
	extract_shader.set_uniform1i("NormalZ", 0);

/*	To do: port this to uniform buffer
	for (int i = 0; i < 160; i++)
	{
		Vec3f sampleVector(random_value(), random_value(), random_value());
		float sampleWeight = 0.2f;//(1.733f - sampleVector.length()) * 0.3f;
		extract_shader.set_uniform3f(string_format("SampleVector[%1]", i), sampleVector * 0.4f);
		extract_shader.set_uniform1f(string_format("SampleWeight[%1]", i), sampleWeight);
	}
*/

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
	rect_primarray = PrimitivesArray(gc);
	rect_primarray.set_attributes(0, rect_positions);

	blur.input = ssao_contribution;

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	blend_state = BlendState(gc, blend_desc);
}

void SSAOPass::run(GraphicContext &gc)
{
	Size viewport_size = normal_z_gbuffer->get_size();
	if (ssao_contribution->is_null() || ssao_contribution->get_size() != viewport_size || !gc.is_frame_buffer_owner(fb))
	{
		ssao_contribution.set(Texture2D());
		fb = FrameBuffer();
		gc.flush();

		ssao_contribution.set(Texture2D(gc, viewport_size.width / 2, viewport_size.height / 2, tf_r8));
		ssao_contribution->set_min_filter(filter_linear);
		ssao_contribution->set_mag_filter(filter_linear);

		fb = FrameBuffer(gc);
		fb.attach_color(0, ssao_contribution.get());
/*		To do: port this to uniform buffer
		float aspect = viewport_size.width/(float)viewport_size.height;
		float field_of_view_y_degrees = 60.0f;
		float field_of_view_y_rad = (float)(field_of_view_y_degrees * M_PI / 180.0);
		float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
		extract_shader.set_uniform1f("f", f);
		extract_shader.set_uniform1f("f_div_aspect", f/aspect);
*/

		blur.output.set(fb);
	}

	normal_z_gbuffer->set_min_filter(filter_nearest);
	normal_z_gbuffer->set_mag_filter(filter_nearest);

	gc.set_frame_buffer(fb);
	gc.set_viewport(ssao_contribution->get_size());
	gc.set_texture(0, normal_z_gbuffer.get());
	gc.set_blend_state(blend_state);
	gc.set_program_object(extract_shader);
	gc.draw_primitives(type_triangles, 6, rect_primarray);
	gc.reset_program_object();
	gc.reset_frame_buffer();

	blur.blur(gc, tf_r8, 2.0f, 7);
}

float SSAOPass::random_value()
{
	float v = rand() / (float)RAND_MAX;
	return v * 2.0f - 1.0f;
}
