
#include "precomp.h"
#include "vsm_shadow_map_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Scene/scene_light_impl.h"

using namespace uicore;

VSMShadowMapPass::VSMShadowMapPass(const GraphicContextPtr &gc, SceneRender &inout)
: inout(inout), maps(gc, inout, 1024, 8, tf_rg32f), round_robin(0)
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

void VSMShadowMapPass::run()
{
	find_lights();
	assign_shadow_map_indexes();
	render_maps();
	blur_maps();
}

void VSMShadowMapPass::find_lights()
{
	lights.clear();

	Size viewport_size = inout.viewport.size();

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	inout.scene->foreach_light(frustum, [&](SceneLightImpl *light)
	{
		// Create data always needed by lightsource pass:

		if (!light->vsm_data)
			light->vsm_data.reset(new VSMShadowMapPassLightData(this, light));

		Quaternionf inv_orientation = Quaternionf::inverse(light->orientation());
		light->vsm_data->world_to_eye = inv_orientation.to_matrix() * Mat4f::translate(-light->position());

		if (light->type() == SceneLight::type_spot)
		{
			float field_of_view = light->falloff();
			light->vsm_data->eye_to_projection = Mat4f::perspective(field_of_view, light->aspect_ratio(), 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
		}
		else if (light->type() == SceneLight::type_directional)
		{
			light->vsm_data->eye_to_projection = Mat4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
		}
		else
		{
			light->vsm_data->eye_to_projection = Mat4f::identity();
		}

		light->vsm_data->world_to_shadow_projection = light->vsm_data->eye_to_projection * light->vsm_data->world_to_eye;

		// Only spot lights with shadow casting enabled uses shadow maps right now, so only generate for those
		if (light->type() == SceneLight::type_spot && light->shadow_caster())
		{
			// skip shadow maps far away
			//float dist = (world_to_eye * Vec4f(light->position, 1.0f)).length3() - light->attenuation_end;
			//if (dist < 80.0f)
			{
				lights.push_back(light);
			}
		}
	});
}

void VSMShadowMapPass::assign_shadow_map_indexes()
{
	maps.start_frame();
	for (size_t i = 0; i < lights.size(); i++)
		lights[i]->vsm_data->shadow_map.use_in_frame();
	maps.assign_indexes();
}

void VSMShadowMapPass::render_maps()
{
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);

	const size_t max_lights_per_frame = 4;
	round_robin += max_lights_per_frame;

	blur_indexes.clear();
	for (size_t j = 0; j < max_lights_per_frame && j < lights.size(); j++)
	{
		size_t i = (round_robin + j) % lights.size();

		if (lights[i]->vsm_data->shadow_map.get_index() != -1)
		{
			inout.gc->set_frame_buffer(lights[i]->vsm_data->shadow_map.get_framebuffer());
			inout.gc->set_viewport(lights[i]->vsm_data->shadow_map.get_view()->size(), inout.gc->texture_image_y_axis());
			inout.gc->clear_depth(1.0f);

			float field_of_view = lights[i]->falloff();
			Mat4f eye_to_cull_projection = Mat4f::perspective(field_of_view, lights[i]->aspect_ratio(), 0.1f, lights[i]->attenuation_end() + 5.0f, handed_left, clip_negative_positive_w);

			FrustumPlanes frustum(eye_to_cull_projection * lights[i]->vsm_data->world_to_eye);

			inout.model_render.clear(inout.scene);
			inout.scene->foreach_object(frustum, [&](SceneObjectImpl *object)
			{
				inout.model_render.add_instance(object);
			});
			inout.model_render.upload(lights[i]->vsm_data->world_to_eye, lights[i]->vsm_data->eye_to_projection);
			inout.model_render.render_shadow();

			blur_indexes.push_back(i);
		}
	}

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
}

void VSMShadowMapPass::blur_maps()
{
	for (size_t j = 0; j < blur_indexes.size(); j++)
	{
		size_t i = blur_indexes[j];
		if (lights[i]->vsm_data->shadow_map.get_index() != -1)
		{
			auto view_texture = lights[i]->vsm_data->shadow_map.get_view();
			auto fb_view = lights[i]->vsm_data->shadow_map.get_framebuffer();
			const auto &fb_blur = lights[i]->vsm_data->shadow_map.fb_blur();
			const auto &blur_texture = lights[i]->vsm_data->shadow_map.blur_texture();

			inout.blur.horizontal(inout.gc, 1.3f, 9, view_texture, fb_blur);
			inout.blur.vertical(inout.gc, 1.3f, 9, blur_texture, fb_view);
		}
	}
}
