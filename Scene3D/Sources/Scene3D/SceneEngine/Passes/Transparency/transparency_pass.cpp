
#include "precomp.h"
#include "transparency_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Performance/scope_timer.h"

using namespace uicore;

TransparencyPass::TransparencyPass(SceneRender &inout) : inout(inout)
{
}

void TransparencyPass::run()
{
	ScopeTimeFunction();

	setup();

	inout.gc->set_frame_buffer(inout.frames.front()->fb_final_color);

	Size viewport_size = inout.viewport_size;
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());

	inout.gc->set_depth_range(0.0f, 0.9f);
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	inout.model_render.begin(inout.scene, inout.world_to_eye, eye_to_projection, ModelRenderMode::transparency);
	inout.scene->foreach_object(frustum, [&](SceneObjectImpl *object)
	{
		inout.model_render.render(object);
	});
	inout.model_render.end();

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

	inout.gc->reset_frame_buffer();
}

void TransparencyPass::setup()
{
	if (!blend_state)
	{
		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		//blend_desc.set_blend_function(blend_one, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
		blend_state = inout.gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = inout.gc->create_depth_stencil_state(depth_stencil_desc);
	}
}
