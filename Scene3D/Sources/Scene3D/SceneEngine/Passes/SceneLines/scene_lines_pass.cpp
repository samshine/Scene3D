
#include "precomp.h"
#include "scene_lines_pass.h"
#include "Scene3D/SceneEngine/scene_viewport_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Scene/scene_light_impl.h"
#include "vertex_scene_lines_hlsl.h"
#include "vertex_scene_lines_glsl.h"
#include "fragment_scene_lines_hlsl.h"
#include "fragment_scene_lines_glsl.h"

using namespace uicore;

SceneLinesPass::SceneLinesPass(SceneRender &inout) : inout(inout)
{
}

void SceneLinesPass::run()
{
	ScopeTimeFunction();

	setup();

	Size viewport_size = inout.viewport.size();
	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());

	if (inout.scene_viewport->_scene_lines.points.empty())
		return;

	if (buffer_size < (int)inout.scene_viewport->_scene_lines.points.size())
	{
		buffer_size = (int)inout.scene_viewport->_scene_lines.points.size();
		positions = VertexArrayVector<Vec3f>(inout.gc, buffer_size);
		colors = VertexArrayVector<Vec3f>(inout.gc, buffer_size);

		prim_array = PrimitivesArray::create(inout.gc);
		prim_array->set_attributes(0, positions);
		prim_array->set_attributes(1, colors);
	}

	positions.upload_data(inout.gc, 0, inout.scene_viewport->_scene_lines.points);
	colors.upload_data(inout.gc, 0, inout.scene_viewport->_scene_lines.colors);

	UniformBlock uniforms;
	uniforms.world_to_projection = eye_to_projection * inout.world_to_eye;
	gpu_uniforms.upload_data(inout.gc, &uniforms, 1);

	inout.gc->set_frame_buffer(inout.fb_final_color);
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());

	inout.gc->set_depth_range(0.0f, 0.9f);
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);

	inout.gc->set_program_object(program);
	inout.gc->set_rasterizer_state(rasterizer_state);

	inout.gc->set_uniform_buffer(0, gpu_uniforms);

	inout.gc->set_primitives_array(prim_array);
	inout.gc->draw_primitives_array(type_lines, 0, (int)inout.scene_viewport->_scene_lines.points.size());
	inout.gc->reset_primitives_array();

	inout.gc->reset_uniform_buffer(0);

	inout.gc->reset_program_object();
	inout.gc->reset_rasterizer_state();
	inout.gc->reset_blend_state();
	inout.gc->reset_depth_stencil_state();
	inout.gc->set_depth_range(0.0f, 1.0f);

	inout.gc->reset_frame_buffer();
}

void SceneLinesPass::setup()
{
	if (!program)
	{
		auto vertex_shader = ShaderObject::create(inout.gc, ShaderType::vertex, inout.gc->shader_language() == shader_hlsl ? vertex_scene_lines_hlsl() : vertex_scene_lines_glsl());
		vertex_shader->compile();

		auto fragment_shader = ShaderObject::create(inout.gc, ShaderType::fragment, inout.gc->shader_language() == shader_hlsl ? fragment_scene_lines_hlsl() : fragment_scene_lines_glsl());
		fragment_shader->compile();

		program = ProgramObject::create(inout.gc);
		program->attach(vertex_shader);
		program->attach(fragment_shader);
		program->bind_attribute_location(0, "AttrPosition");
		program->bind_attribute_location(1, "AttrColor");
		program->bind_frag_data_location(0, "FragColor");
		if (!program->try_link())
			throw Exception(string_format("Lines program failed to link: %1", program->get_info_log()));
		program->set_uniform_buffer_index("Uniforms", 0);

		gpu_uniforms = uicore::UniformVector<UniformBlock>(inout.gc, 1);

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
		blend_state = inout.gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_state = inout.gc->create_depth_stencil_state(depth_stencil_desc);

		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_culled(false);
		rasterizer_desc.set_polygon_offset(0.0f, 10.0f);
		rasterizer_desc.set_face_fill_mode(fill_line);
		rasterizer_desc.enable_line_antialiasing(true);
		rasterizer_state = inout.gc->create_rasterizer_state(rasterizer_desc);
	}
}
