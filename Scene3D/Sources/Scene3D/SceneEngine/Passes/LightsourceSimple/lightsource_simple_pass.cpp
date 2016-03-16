
#include "precomp.h"
#include "lightsource_simple_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/Passes/ShadowMap/shadow_map_pass.h"
#include "Scene3D/Scene/scene_camera_impl.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/scene.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Performance/gpu_timer.h"
#include <algorithm>
#include "vertex_icosahedron_hlsl.h"
#include "vertex_icosahedron_glsl.h"
#include "vertex_rect_hlsl.h"
#include "vertex_rect_glsl.h"
#include "fragment_light_hlsl.h"
#include "fragment_light_glsl.h"

using namespace uicore;

LightsourceSimplePass::LightsourceSimplePass(SceneRender &inout) : inout(inout)
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		icosahedron_light_program = compile_and_link(inout.gc, vertex_icosahedron_glsl(), fragment_light_glsl());
		rect_light_program = compile_and_link(inout.gc, vertex_rect_glsl(), fragment_light_glsl(), "RECT_PASS");
	}
	else
	{
		icosahedron_light_program = compile_and_link(inout.gc, vertex_icosahedron_hlsl(), fragment_light_hlsl());
		rect_light_program = compile_and_link(inout.gc, vertex_rect_hlsl(), fragment_light_hlsl(), "RECT_PASS");
	}

	light_instance_texture = Texture1D::create(inout.gc, max_lights * vectors_per_light, tf_rgba32f);
	light_instance_transfer = PixelBuffer::create(max_lights * vectors_per_light, 1, tf_rgba32f);

	icosahedron.reset(new Icosahedron(inout.gc, true));
}

LightsourceSimplePass::~LightsourceSimplePass()
{
}

ProgramObjectPtr LightsourceSimplePass::compile_and_link(const GraphicContextPtr &gc, const std::string &vertex_code, const std::string &fragment_code, const std::string &defines)
{
	auto program = ShaderSetup::compile(gc, "lightsource simple", vertex_code, fragment_code, defines);

	program->bind_frag_data_location(0, "FragColor");

	if (!program->try_link())
		throw Exception(string_format("Light source simple shader linking failed: %1", program->info_log()));

	program->bind_attribute_location(0, "AttrPositionInObject");
	program->set_uniform_buffer_index("Uniforms", 0);
	program->set_uniform1i("InstanceTexture", 0);
	program->set_uniform1i("NormalZTexture", 1);
	program->set_uniform1i("DiffuseColorTexture", 2);
	program->set_uniform1i("SpecularColorTexture", 3);
	program->set_uniform1i("SpecularLevelTexture", 4);
	program->set_uniform1i("ShadowMapsTexture", 5);
	program->set_uniform1i("ShadowMapsTextureSampler", 5);
	program->set_uniform1i("SelfIlluminationTexture", 6);

	return program;
}

void LightsourceSimplePass::setup()
{
	Size viewport_size = inout.viewport.size();
	if (!blend_state)
	{
		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_one, blend_one, blend_one, blend_one);
		blend_state = inout.gc->create_blend_state(blend_desc);

		DepthStencilStateDescription icosahedron_depth_stencil_desc;
		icosahedron_depth_stencil_desc.enable_depth_write(false);
		icosahedron_depth_stencil_desc.enable_depth_test(true);
		icosahedron_depth_stencil_desc.set_depth_compare_function(compare_lequal);
		icosahedron_depth_stencil_state = inout.gc->create_depth_stencil_state(icosahedron_depth_stencil_desc);

		RasterizerStateDescription icosahedron_rasterizer_desc;
		icosahedron_rasterizer_desc.set_culled(true);
		icosahedron_rasterizer_state = inout.gc->create_rasterizer_state(icosahedron_rasterizer_desc);

		DepthStencilStateDescription rect_depth_stencil_desc;
		rect_depth_stencil_desc.enable_depth_write(false);
		rect_depth_stencil_desc.enable_depth_test(false);
		rect_depth_stencil_state = inout.gc->create_depth_stencil_state(rect_depth_stencil_desc);

		RasterizerStateDescription rect_rasterizer_desc;
		rect_rasterizer_desc.set_culled(false);
		rect_rasterizer_state = inout.gc->create_rasterizer_state(rect_rasterizer_desc);

		uniforms = UniformVector<Uniforms>(inout.gc, 1);

		icosahedron_prim_array = PrimitivesArray::create(inout.gc);
		icosahedron_prim_array->set_attributes(0, icosahedron->vertices);

		Vec4f positions[6] =
		{
			Vec4f(-1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
			Vec4f( 1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
			Vec4f( 1.0f,  1.0f, 1.0f, 1.0f)
		};
		rect_positions = VertexArrayVector<Vec4f>(inout.gc, positions, 6);

		rect_prim_array = PrimitivesArray::create(inout.gc);
		rect_prim_array->set_attributes(0, rect_positions);
	}
}

void LightsourceSimplePass::run()
{
	ScopeTimeFunction();
	setup();
	find_lights();
	upload();
	render();
}

void LightsourceSimplePass::find_lights()
{
	lights.clear();

	Size viewport_size = inout.viewport.size();

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);
	inout.scene->foreach_light(frustum, [&](SceneLightImpl *light)
	{
		if ((light->type() == SceneLight::type_omni || light->type() == SceneLight::type_spot) && light->light_caster() && lights.size() < max_lights - 1)
		{
			lights.push_back(light);
		}
	});
}

void LightsourceSimplePass::upload()
{
	ScopeTimeFunction();

	Size viewport_size = inout.viewport.size();
	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e4f, handed_left, inout.gc->clip_z_range());

	float aspect = inout.viewport.width()/(float)inout.viewport.height();
	float field_of_view_y_degrees = inout.field_of_view;
	float field_of_view_y_rad = (float)(field_of_view_y_degrees * PI / 180.0);
	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
	float rcp_f = 1.0f / f;
	float rcp_f_div_aspect = 1.0f / (f / aspect);
	Vec2f two_rcp_viewport_size(2.0f / inout.viewport.width(), 2.0f / inout.viewport.height());

	Uniforms cpu_uniforms;
	cpu_uniforms.eye_to_projection = eye_to_projection;
	cpu_uniforms.object_to_eye = Quaternionf::inverse(inout.camera->orientation()).to_matrix();
	cpu_uniforms.rcp_f = rcp_f;
	cpu_uniforms.rcp_f_div_aspect = rcp_f_div_aspect;
	cpu_uniforms.two_rcp_viewport_size = two_rcp_viewport_size;
	uniforms.upload_data(inout.gc, &cpu_uniforms, 1);

	int num_lights = lights.size();

	Mat4f normal_world_to_eye = Mat4f(Mat3f(inout.world_to_eye)); // This assumes uniform scale
	Mat4f eye_to_world = Mat4f::inverse(inout.world_to_eye);

	Vec4f *instance_data = light_instance_transfer->data<Vec4f>();

	for (int i = 0; i < num_lights; i++)
	{
		float radius = lights[i]->attenuation_end();
		if (lights[i]->rectangle_shape())
			radius *= 1.414213562373f;

		float attenuation_delta = lights[i]->attenuation_end() - lights[i]->attenuation_start();
		if (attenuation_delta == 0.0f)
			attenuation_delta = 1e-6f;
		float sqr_radius = radius * radius;
#ifdef USE_QUADRATIC_ATTENUATION
		float sqr_attenuation_start = lights[i]->attenuation_start * lights[i]->attenuation_start;
		float sqr_attenuation_delta = attenuation_delta * attenuation_delta;
#else
		float attenuation_start = lights[i]->attenuation_start();
#endif
		float sqr_falloff_begin = 0.0f;
		float light_type = 0.0f;
		if (lights[i]->type() == SceneLight::type_spot)
		{
			light_type = lights[i]->rectangle_shape() ? 2.0f : 1.0f;
			float falloff_begin = lights[i]->hotspot() / lights[i]->falloff();
			sqr_falloff_begin = falloff_begin * falloff_begin;
		}
		Vec3f position_in_eye = Vec3f(inout.world_to_eye * Vec4f(lights[i]->position(), 1.0f));
		Mat4f eye_to_shadow_projection = lights[i]->world_to_shadow_projection(inout.gc->clip_z_range()) * eye_to_world;

		instance_data[i * vectors_per_light + 0] = Vec4f(position_in_eye, (float)lights[i]->shadow_map_index);
		instance_data[i * vectors_per_light + 1] = Vec4f(lights[i]->color(), lights[i]->ambient_illumination());
#ifdef USE_QUADRATIC_ATTENUATION
		instance_data[i * vectors_per_light + 2] = Vec4f(sqr_radius, sqr_attenuation_start, 1.0f / sqr_attenuation_delta, sqr_falloff_begin);
#else
		instance_data[i * vectors_per_light + 2] = Vec4f(sqr_radius, attenuation_start, 1.0f / attenuation_delta, sqr_falloff_begin);
#endif
		instance_data[i * vectors_per_light + 3] = Vec4f(eye_to_shadow_projection[0], eye_to_shadow_projection[4], eye_to_shadow_projection[8], light_type);
		instance_data[i * vectors_per_light + 4] = Vec4f(eye_to_shadow_projection[1], eye_to_shadow_projection[5], eye_to_shadow_projection[9], 0.0f);
		instance_data[i * vectors_per_light + 5] = Vec4f(eye_to_shadow_projection[2], eye_to_shadow_projection[6], eye_to_shadow_projection[10], 0.0f);
	}

	instance_data[num_lights * vectors_per_light + 0] = Vec4f(0.0f);
	instance_data[num_lights * vectors_per_light + 1] = Vec4f(0.0f);
	instance_data[num_lights * vectors_per_light + 2] = Vec4f(0.0f);
	instance_data[num_lights * vectors_per_light + 3] = Vec4f(0.0f);
	instance_data[num_lights * vectors_per_light + 4] = Vec4f(0.0f);
	instance_data[num_lights * vectors_per_light + 5] = Vec4f(0.0f);

	light_instance_texture->set_image(inout.gc, light_instance_transfer);
}

void LightsourceSimplePass::render()
{
	ScopeTimeFunction();

	//inout.timer.begin_time(inout.gc, "light(simple)");

	inout.gc->set_frame_buffer(inout.fb_final_color);

	inout.gc->set_viewport(inout.viewport.size(), inout.gc->texture_image_y_axis());

	inout.gc->set_depth_range(0.0f, 0.9f);

	inout.gc->set_uniform_buffer(0, uniforms);
	inout.gc->set_texture(0, light_instance_texture);
	inout.gc->set_texture(1, inout.normal_z_gbuffer);
	inout.gc->set_texture(2, inout.diffuse_color_gbuffer);
	inout.gc->set_texture(3, inout.specular_color_gbuffer);
	inout.gc->set_texture(4, inout.specular_level_gbuffer);
	inout.gc->set_texture(5, inout.shadow_maps.shadow_maps);
	inout.gc->set_texture(6, inout.self_illumination_gbuffer);

	inout.gc->set_blend_state(blend_state);

	inout.gc->clear();

	// To do: use icosahedron for smaller lights and when the camera is not inside the light influence sphere
	// To do: combine multiple lights into the same rect pass to reduce overdraw penalty

#if !RENDER_WITH_ICOSAHEDRON
	inout.gc->set_depth_stencil_state(rect_depth_stencil_state);
	inout.gc->set_rasterizer_state(rect_rasterizer_state);
	inout.gc->set_program_object(rect_light_program);

	inout.gc->set_primitives_array(rect_prim_array);
	inout.gc->draw_primitives_array_instanced(type_triangles, 0, 6, std::max(lights.size(), (size_t)1));
	inout.gc->reset_primitives_array();
#else
	inout.gc->set_depth_stencil_state(icosahedron_depth_stencil_state);
	inout.gc->set_rasterizer_state(icosahedron_rasterizer_state);
	inout.gc->set_program_object(icosahedron_light_program);

	inout.gc->set_primitives_array(icosahedron_prim_array);
	inout.gc->draw_primitives_elements_instanced(type_triangles, icosahedron->num_elements, icosahedron->elements, type_unsigned_int, 0, lights.size());
	inout.gc->reset_primitives_array();
#endif

	//inout.timer.end_time(inout.gc);
	//inout.timer.begin_time(inout.gc, "light(simple)");

	inout.gc->reset_texture(6);
	inout.gc->reset_texture(5);
	inout.gc->reset_texture(4);
	inout.gc->reset_texture(3);
	inout.gc->reset_texture(2);
	inout.gc->reset_texture(1);
	inout.gc->reset_texture(0);
	inout.gc->reset_uniform_buffer(0);

	//inout.timer.end_time(inout.gc);
}
