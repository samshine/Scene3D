
#include "precomp.h"
#include "shadow_map_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Scene/scene_light_impl.h"
#include "Scene3D/Performance/scope_timer.h"

using namespace uicore;

ShadowMapPass::ShadowMapPass(const GraphicContextPtr &gc, SceneRender &inout) : inout(inout)
{
	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	blend_state = gc->create_blend_state(blend_desc);

	DepthStencilStateDescription depth_stencil_desc;
	depth_stencil_desc.enable_depth_write(true);
	depth_stencil_desc.enable_depth_test(true);
	depth_stencil_desc.set_depth_compare_function(compare_lequal);
	depth_stencil_state = gc->create_depth_stencil_state(depth_stencil_desc);
}

void ShadowMapPass::run()
{
	ScopeTimeFunction();

	inout.shadow_maps.setup(1024, 8, tf_rg32f);

	Size viewport_size = inout.viewport.size();

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	inout.shadow_maps.start_frame();
	inout.scene->foreach_light(frustum, [&](SceneLightImpl *light)
	{
		// Only spot lights with shadow casting enabled uses shadow maps right now, so only generate for those
		if (light->type() == SceneLight::type_spot && light->shadow_caster())
			inout.shadow_maps.add_light(light);
	});

	for (auto &slot : inout.shadow_maps.lights)
	{
		if (!slot.light)
			continue;

		render_map(slot);
	}
}

void ShadowMapPass::render_map(ShadowMapLight &slot)
{
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);

	inout.gc->set_frame_buffer(slot.framebuffer);
	inout.gc->set_viewport(slot.view->size(), inout.gc->texture_image_y_axis());
	inout.gc->clear_depth(1.0f);

	float field_of_view = slot.light->falloff();
	Mat4f eye_to_cull_projection = Mat4f::perspective(field_of_view, slot.light->aspect_ratio(), 0.1f, slot.light->attenuation_end() + 5.0f, handed_left, clip_negative_positive_w);

	FrustumPlanes frustum(eye_to_cull_projection * slot.light->world_to_shadow());

	inout.model_render.clear(inout.scene);
	inout.scene->foreach_object(frustum, [&](SceneObjectImpl *object)
	{
		inout.model_render.add_instance(object);
	});
	inout.model_render.upload(slot.light->world_to_shadow(), slot.light->shadow_to_projection(inout.gc->clip_z_range()));
	inout.model_render.render_shadow();

	inout.gc->reset_rasterizer_state();
	inout.gc->reset_depth_stencil_state();

	inout.gc->reset_program_object();
	inout.gc->reset_primitives_elements();
	inout.gc->reset_texture(0);
	inout.gc->reset_texture(1);
	inout.gc->reset_texture(2);
	inout.gc->reset_texture(3);
	inout.gc->reset_texture(4);
	inout.gc->reset_texture(5);
	inout.gc->reset_uniform_buffer(0);
	inout.gc->reset_uniform_buffer(1);
	inout.gc->reset_frame_buffer();

	inout.blur.horizontal(inout.gc, 1.3f, 9, slot.view, inout.shadow_maps.fb_blur);
	inout.blur.vertical(inout.gc, 1.3f, 9, inout.shadow_maps.blur_texture, slot.framebuffer);
}
