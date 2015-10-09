
#include "precomp.h"
#include "transparency_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Model/model_lod.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

TransparencyPass::TransparencyPass(ResourceContainer &inout)
{
	viewport = inout.get<Rect>("Viewport");
	field_of_view = inout.get<float>("FieldOfView");
	world_to_eye = inout.get<Mat4f>("WorldToEye");
	zbuffer = inout.get<Texture2DPtr>("ZBuffer");

	final_color = inout.get<Texture2DPtr>("FinalColor");
}

void TransparencyPass::run(const GraphicContextPtr &render_gc, Scene_Impl *render_scene)
{
	gc = render_gc;
	scene = render_scene;
	setup(gc);

	gc->set_frame_buffer(fb_transparency);

	Size viewport_size = viewport->get_size();
	gc->set_viewport(viewport_size, gc->texture_image_y_axis());

	gc->set_depth_range(0.0f, 0.9f);
	gc->set_depth_stencil_state(depth_stencil_state);
	gc->set_blend_state(blend_state);

	Mat4f eye_to_projection = Mat4f::perspective(field_of_view.get(), viewport_size.width/(float)viewport_size.height, 0.1f, 1.e10f, handed_left, gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(field_of_view.get(), viewport_size.width/(float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * world_to_eye.get());
	scene->visit(gc, world_to_eye.get(), eye_to_projection, frustum, this);

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
	Size viewport_size = viewport->get_size();
	if (!fb_transparency || !gc->is_frame_buffer_owner(fb_transparency) || final_color.updated() || zbuffer.updated())
	{
		fb_transparency = FrameBuffer::create(gc);
		fb_transparency->attach_color(0, final_color.get());
		fb_transparency->attach_depth(zbuffer.get());

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_one, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_state = gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = gc->create_depth_stencil_state(depth_stencil_desc);
	}
}

void TransparencyPass::render(const GraphicContextPtr &gc, ModelLOD *model_lod, int num_instances)
{
	model_lod->transparency_commands.execute(scene, gc, num_instances);
}
