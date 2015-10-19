
#include "precomp.h"
#include "gbuffer_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/SceneEngine/Model/model_lod.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

GBufferPass::GBufferPass(SceneRender &inout) : inout(inout)
{
}

void GBufferPass::run(const GraphicContextPtr &render_gc, SceneImpl *render_scene)
{
	gc = render_gc;
	scene = render_scene;
	setup_gbuffer(gc);

	gc->set_frame_buffer(inout.fb_gbuffer);

	if (gc->shader_language() == shader_glsl)
	{
		// To do: support this in clanlib
		OpenGL::set_active(gc);
		uicore::GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, buffers);
	}

	Size viewport_size = inout.viewport.size();
	gc->set_viewport(viewport_size, gc->texture_image_y_axis());

	gc->clear_depth(1.0f);
	gc->clear(); // Needed by skybox pass

	gc->set_depth_range(0.0f, 0.9f);
	gc->set_depth_stencil_state(depth_stencil_state);
	gc->set_blend_state(early_z_blend_state);

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 1.e10f, handed_left, gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	render_list.clear();
	inout.instances_buffer.render_pass(gc, scene, inout.world_to_eye, eye_to_projection, frustum, [&](ModelLOD *model_lod, int num_instances)
	{
		model_lod->early_z_commands.execute(scene, gc, num_instances);
		render_list.push_back(RenderEntry(model_lod, num_instances));
	});

	gc->set_blend_state(blend_state);
	for (size_t i = 0; i < render_list.size(); i++)
		render_list[i].model_lod->gbuffer_commands.execute(scene, gc, render_list[i].num_instances);

	if (gc->shader_language() == shader_glsl)
	{
		// To do: support this in ClanLib
		OpenGL::set_active(gc);
		glDisablei(GL_BLEND, 0);
		glDisablei(GL_BLEND, 1);
		glDisablei(GL_BLEND, 2);
		glDisablei(GL_BLEND, 3);
	}

	gc->reset_rasterizer_state();
	gc->reset_depth_stencil_state();
	gc->set_depth_range(0.0f, 1.0f);

	gc->reset_program_object();
	gc->reset_primitives_elements();
	gc->reset_texture(0);
	gc->reset_texture(1);
	gc->reset_texture(2);
	gc->reset_texture(3);
	gc->reset_uniform_buffer(0);
	gc->reset_uniform_buffer(1);

	if (gc->shader_language() == shader_glsl)
	{
		OpenGL::set_active(gc);
		uicore::GLenum buffers2[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers2);
	}
	gc->reset_frame_buffer();

	gc.reset();
}

void GBufferPass::setup_gbuffer(const GraphicContextPtr &gc)
{
	if (!blend_state)
	{
		BlendStateDescription blend_desc;
		blend_desc.enable_blending(false);
		blend_desc.set_blend_function(blend_one, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_state = gc->create_blend_state(blend_desc);

		BlendStateDescription early_z_blend_desc;
		early_z_blend_desc.enable_blending(false);
		early_z_blend_desc.enable_color_write(false, false, false, false);
		early_z_blend_state = gc->create_blend_state(early_z_blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(true);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = gc->create_depth_stencil_state(depth_stencil_desc);
	}
}
