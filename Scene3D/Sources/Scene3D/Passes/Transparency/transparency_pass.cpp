
#include "precomp.h"
#include "transparency_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Model/model_lod.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

TransparencyPass::TransparencyPass(ResourceContainer &inout) : inout(inout)
{
}

void TransparencyPass::run(const GraphicContextPtr &render_gc, SceneImpl *render_scene)
{
	gc = render_gc;
	scene = render_scene;
	setup(gc);

	gc->set_frame_buffer(inout.fb_final_color);

	Size viewport_size = inout.viewport.size();
	gc->set_viewport(viewport_size, gc->texture_image_y_axis());

	gc->set_depth_range(0.0f, 0.9f);
	gc->set_depth_stencil_state(depth_stencil_state);
	gc->set_blend_state(blend_state);

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 1.e10f, handed_left, gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	scene->instances_buffer.render_pass(gc, scene, inout.world_to_eye, eye_to_projection, frustum, [&](ModelLOD *model_lod, int num_instances)
	{
		model_lod->transparency_commands.execute(scene, gc, num_instances);
	});

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

	gc->reset_frame_buffer();

	gc.reset();
}

void TransparencyPass::setup(const GraphicContextPtr &gc)
{
	if (!blend_state)
	{
		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		//blend_desc.set_blend_function(blend_one, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
		blend_state = gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = gc->create_depth_stencil_state(depth_stencil_desc);
	}
}
