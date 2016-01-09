
#include "precomp.h"
#include "lightsource_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/mapped_buffer.h"
#include "Scene3D/SceneEngine/Passes/ShadowMap/shadow_map_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Performance/gpu_timer.h"
#include "Scene3D/Scene/scene_impl.h"
#include <algorithm>
#include "cull_tiles_glsl.h"
#include "cull_tiles_hlsl.h"
#include "render_tiles_glsl.h"
#include "render_tiles_hlsl.h"
#include "log_event.h"

using namespace uicore;

LightsourcePass::LightsourcePass(const GraphicContextPtr &gc, SceneRender &inout) : inout(inout)
{
	if (gc->shader_language() == shader_glsl)
	{
		cull_tiles_program = compile_and_link(gc, "cull tiles", cull_tiles_glsl());
		render_tiles_program = compile_and_link(gc, "render tiles", render_tiles_glsl());
	}
	else
	{
		cull_tiles_program = compile_and_link(gc, "cull tiles", cull_tiles_hlsl());
		render_tiles_program = compile_and_link(gc, "render tiles", render_tiles_hlsl());
	}

	compute_uniforms = UniformVector<Uniforms>(gc, 1);
	compute_lights = StorageVector<GPULight>(gc, max_lights);
	for (int i = 0; i < num_transfer_lights; i++)
		transfer_lights[i] = StagingVector<GPULight>(gc, max_lights);
}

LightsourcePass::~LightsourcePass()
{
}

void LightsourcePass::run()
{
	ScopeTimeFunction();
	find_lights();
	upload();
	render();
}

void LightsourcePass::find_lights()
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
#ifdef TEST_LIGHT_DISTANCE
			float dist = (world_to_eye * Vec4f(light->position, 1.0f)).length3() - light->attenuation_end;
			if (dist < 30.0f)
				lights.push_back(light);
#else
			lights.push_back(light);
#endif
		}
	});
}

class LightsourcePass_LightCompare
{
public:
	bool operator()(SceneLightImpl *a, SceneLightImpl *b)
	{
		bool a_has_shadow = a->shadow_map_index >= 0;
		bool b_has_shadow = b->shadow_map_index >= 0;

		if (a_has_shadow != b_has_shadow)
			return a_has_shadow < b_has_shadow;
		else
			return a->type() < b->type();
	}
};

void LightsourcePass::upload()
{
	ScopeTimeFunction();

	int num_lights = lights.size();

	std::sort(lights.begin(), lights.end(), LightsourcePass_LightCompare());

	float aspect = inout.viewport.width()/(float)inout.viewport.height();
	float field_of_view_y_degrees = inout.field_of_view;
	float field_of_view_y_rad = (float)(field_of_view_y_degrees * PI / 180.0);
	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
	float rcp_f = 1.0f / f;
	float rcp_f_div_aspect = 1.0f / (f / aspect);
	Vec2f two_rcp_viewport_size(2.0f / inout.viewport.width(), 2.0f / inout.viewport.height());

	Uniforms uniforms;
	uniforms.rcp_f = rcp_f;
	uniforms.rcp_f_div_aspect = rcp_f_div_aspect;
	uniforms.two_rcp_viewport_size = two_rcp_viewport_size;
	uniforms.num_lights = num_lights;
	uniforms.num_tiles_x = num_tiles_x;
	uniforms.num_tiles_y = num_tiles_y;
	uniforms.scene_ambience = Vec4f(0.02f, 0.02f, 0.022f, 0.0f);
	compute_uniforms.upload_data(inout.gc, &uniforms, 1);

	Mat4f normal_world_to_eye = Mat4f(Mat3f(inout.world_to_eye)); // This assumes uniform scale
	Mat4f eye_to_world = Mat4f::inverse(inout.world_to_eye);

	transfer_lights[current_transfer_lights].lock(inout.gc, access_write_only);

	MappedBuffer<GPULight> data(transfer_lights[current_transfer_lights].data(), max_lights);
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

		data[i].position = Vec4f(position_in_eye, (float)lights[i]->shadow_map_index);
		data[i].color = Vec4f(lights[i]->color(), lights[i]->ambient_illumination());
#ifdef USE_QUADRATIC_ATTENUATION
		data[i].range = Vec4f(sqr_radius, sqr_attenuation_start, 1.0f / sqr_attenuation_delta, sqr_falloff_begin);
#else
		data[i].range = Vec4f(sqr_radius, attenuation_start, 1.0f / attenuation_delta, sqr_falloff_begin);
#endif
		data[i].spot_x = Vec4f(eye_to_shadow_projection[0], eye_to_shadow_projection[4], eye_to_shadow_projection[8], light_type);
		data[i].spot_y = Vec4f(eye_to_shadow_projection[1], eye_to_shadow_projection[5], eye_to_shadow_projection[9], 0.0f);
		data[i].spot_z = Vec4f(eye_to_shadow_projection[2], eye_to_shadow_projection[6], eye_to_shadow_projection[10], 0.0f);
	}
	data[num_lights].position = Vec4f(0.0f);
	data[num_lights].color = Vec4f(0.0f);
	data[num_lights].range = Vec4f(0.0f);
	transfer_lights[current_transfer_lights].unlock();
	compute_lights.copy_from(inout.gc, transfer_lights[current_transfer_lights], 0, 0, num_lights + 1);
	current_transfer_lights = (current_transfer_lights + 1) % num_transfer_lights;
}

void LightsourcePass::render()
{
	ScopeTimeFunction();

	zminmax.viewport = inout.viewport;
	zminmax.normal_z = inout.normal_z_gbuffer;
	zminmax.minmax(inout.gc);
	update_buffers();

	//inout.timer.begin_time(inout.gc, "light(cull)");

	inout.gc->set_uniform_buffer(0, compute_uniforms);
	inout.gc->set_storage_buffer(0, compute_lights);
	inout.gc->set_storage_buffer(1, compute_visible_lights);
	inout.gc->set_texture(0, zminmax.result);
	inout.gc->set_texture(1, inout.normal_z_gbuffer);
	inout.gc->set_texture(2, inout.diffuse_color_gbuffer);
	inout.gc->set_texture(3, inout.specular_color_gbuffer);
	inout.gc->set_texture(4, inout.specular_level_gbuffer);
	inout.gc->set_texture(5, inout.shadow_maps.shadow_maps);
	inout.gc->set_texture(6, inout.self_illumination_gbuffer);
	inout.gc->set_image_texture(0, inout.final_color);

	inout.gc->set_program_object(cull_tiles_program);
	inout.gc->dispatch((num_tiles_x + tile_size - 1) / tile_size, (num_tiles_y + tile_size - 1) / tile_size);

	//timer.end_time(inout.gc);
	//timer.begin_time(inout.gc, "light(render)");

	inout.gc->set_program_object(render_tiles_program);
	inout.gc->dispatch(num_tiles_x, num_tiles_y);

	inout.gc->reset_image_texture(0);
	inout.gc->reset_texture(6);
	inout.gc->reset_texture(5);
	inout.gc->reset_texture(4);
	inout.gc->reset_texture(3);
	inout.gc->reset_texture(2);
	inout.gc->reset_texture(1);
	inout.gc->reset_texture(0);
	inout.gc->reset_uniform_buffer(2);
	inout.gc->reset_uniform_buffer(1);
	inout.gc->reset_uniform_buffer(0);

	//timer.end_time(inout.gc);
}

void LightsourcePass::update_buffers()
{
	int needed_num_tiles_x = (inout.viewport.width() + tile_size - 1) / tile_size;
	int needed_num_tiles_y = (inout.viewport.height() + tile_size - 1) / tile_size;

	if (!compute_visible_lights.buffer() || num_tiles_x != needed_num_tiles_x || num_tiles_y != needed_num_tiles_y)
	{
		compute_visible_lights = StorageVector<unsigned int>();
		inout.gc->flush();

		num_tiles_x = needed_num_tiles_x;
		num_tiles_y = needed_num_tiles_y;

		compute_visible_lights = StorageVector<unsigned int>(inout.gc, num_tiles_x * num_tiles_y * light_slots_per_tile);
	}
}

ProgramObjectPtr LightsourcePass::compile_and_link(const GraphicContextPtr &gc, const std::string &program_name, const std::string &source, const std::string &defines)
{
	std::string prefix;
	std::vector<std::string> define_list = Text::split(defines, " ");
	if (gc->shader_language() == shader_glsl)
		prefix += "#version 430\r\n";
	for (size_t i = 0; i < define_list.size(); i++)
		prefix += string_format("#define %1\r\n", define_list[i]);
	prefix += "#line 0\r\n";

	auto compute_shader = ShaderObject::create(gc, ShaderType::compute, prefix + source);
	if (!compute_shader->try_compile())
		throw Exception(string_format("Unable to compile %1 compute shader: %2", program_name, compute_shader->info_log()));

	auto program = ProgramObject::create(gc);
	program->attach(compute_shader);
	if (!program->try_link())
		throw Exception(string_format("Failed to link %1: %2", program_name, program->get_info_log()));

	// Uniforms
	program->set_uniform_buffer_index("Uniforms", 0);

	// Storage buffers
	program->set_storage_buffer_index("lights", 0);
	program->set_storage_buffer_index("VisibleLightIndices", 1);

	// Textures
	program->set_uniform1i("zminmax", 0);
	program->set_uniform1i("normal_z", 1);
	program->set_uniform1i("diffuse", 2);
	program->set_uniform1i("specular", 3);
	program->set_uniform1i("specular_level", 4);
	program->set_uniform1i("shadow_maps", 5);
	program->set_uniform1i("self_illumination", 6);

	// Samplers
	program->set_uniform1i("shadow_maps_sampler", 5);

	// Images
	program->set_uniform1i("out_final", 0);

	return program;
}
