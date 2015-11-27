
#include "precomp.h"
#include "gbuffer_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/SceneEngine/scene_viewport_impl.h"
#include "Scene3D/Scene/scene_impl.h"

using namespace uicore;

GBufferPass::GBufferPass(SceneRender &inout) : inout(inout)
{
}

void GBufferPass::run()
{
	setup_gbuffer();

	inout.gc->set_frame_buffer(inout.fb_gbuffer);

	if (inout.gc->shader_language() == shader_glsl)
	{
		// To do: support this in clanlib
		OpenGL::set_active(inout.gc);
		uicore::GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, buffers);
	}

	Size viewport_size = inout.viewport.size();
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());

	inout.gc->clear_depth(1.0f);
	inout.gc->clear(); // Needed by skybox pass

	inout.gc->set_depth_range(0.0f, 0.9f);
	inout.gc->set_depth_stencil_state(depth_stencil_state);

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	inout.model_render.clear(inout.scene);
	inout.scene->foreach_object(frustum, [&](SceneObjectImpl *object)
	{
		inout.model_render.add_instance(object);
	});
	inout.model_render.upload(inout.world_to_eye, eye_to_projection);

	inout.gc->set_blend_state(early_z_blend_state);
	inout.model_render.render_early_z();

	inout.gc->set_blend_state(blend_state);
	inout.model_render.render_gbuffer();

	if (inout.gc->shader_language() == shader_glsl)
	{
		// To do: support this in ClanLib
		OpenGL::set_active(inout.gc);
		glDisablei(GL_BLEND, 0);
		glDisablei(GL_BLEND, 1);
		glDisablei(GL_BLEND, 2);
		glDisablei(GL_BLEND, 3);
	}

	inout.gc->reset_rasterizer_state();
	inout.gc->reset_depth_stencil_state();
	inout.gc->set_depth_range(0.0f, 1.0f);

	inout.gc->reset_program_object();
	inout.gc->reset_primitives_elements();
	inout.gc->reset_texture(0);
	inout.gc->reset_texture(1);
	inout.gc->reset_texture(2);
	inout.gc->reset_texture(3);
	inout.gc->reset_uniform_buffer(0);
	inout.gc->reset_uniform_buffer(1);

	if (inout.gc->shader_language() == shader_glsl)
	{
		OpenGL::set_active(inout.gc);
		uicore::GLenum buffers2[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers2);
	}
	inout.gc->reset_frame_buffer();
}

void GBufferPass::setup_gbuffer()
{
	if (!blend_state)
	{
		BlendStateDescription blend_desc;
		blend_desc.enable_blending(false);
		blend_desc.set_blend_function(blend_one, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_state = inout.gc->create_blend_state(blend_desc);

		BlendStateDescription early_z_blend_desc;
		early_z_blend_desc.enable_blending(false);
		early_z_blend_desc.enable_color_write(false, false, false, false);
		early_z_blend_state = inout.gc->create_blend_state(early_z_blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(true);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = inout.gc->create_depth_stencil_state(depth_stencil_desc);
	}
}
