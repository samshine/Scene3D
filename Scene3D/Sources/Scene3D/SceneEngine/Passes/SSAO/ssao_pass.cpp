
#include "precomp.h"
#include "ssao_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "vertex_ssao_extract_glsl.h"
#include "vertex_ssao_extract_hlsl.h"
#include "fragment_ssao_extract_glsl.h"

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
		//extract_shader = ShaderSetup::compile(gc, "ssao", vertex_ssao_extract_hlsl(), fragment_ssao_extract_hlsl(), "");
	}
	ShaderSetup::link(extract_shader, "ssao extract program");
	extract_shader->bind_attribute_location(0, "AttrPositionInProjection");
	extract_shader->set_uniform1i("NormalZ", 0);

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
	rect_primarray = PrimitivesArray::create(gc);
	rect_primarray->set_attributes(0, rect_positions);

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	blend_state = gc->create_blend_state(blend_desc);
}

void SSAOPass::run(const GraphicContextPtr &gc, SceneImpl *scene)
{
	/*		To do: port this to uniform buffer
	float aspect = viewport_size.width/(float)viewport_size.height;
	float field_of_view_y_degrees = 60.0f;
	float field_of_view_y_rad = (float)(field_of_view_y_degrees * M_PI / 180.0);
	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
	extract_shader.set_uniform1f("f", f);
	extract_shader.set_uniform1f("f_div_aspect", f/aspect);
	*/

	inout.normal_z_gbuffer->set_min_filter(filter_nearest);
	inout.normal_z_gbuffer->set_mag_filter(filter_nearest);

	gc->set_frame_buffer(inout.fb_ambient_occlusion);
	gc->set_viewport(inout.ambient_occlusion->size(), gc->texture_image_y_axis());
	gc->set_texture(0, inout.normal_z_gbuffer);
	gc->set_blend_state(blend_state);
	gc->set_program_object(extract_shader);
	gc->draw_primitives(type_triangles, 6, rect_primarray);
	gc->reset_program_object();
	gc->reset_frame_buffer();

	inout.blur.input = inout.ambient_occlusion;
	inout.blur.output = inout.fb_ambient_occlusion;
	inout.blur.blur(gc, tf_r8, 2.0f, 7);
}

float SSAOPass::random_value()
{
	float v = rand() / (float)RAND_MAX;
	return v * 2.0f - 1.0f;
}
