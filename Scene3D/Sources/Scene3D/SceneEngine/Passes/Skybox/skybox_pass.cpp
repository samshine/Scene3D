
#include "precomp.h"
#include "skybox_pass.h"
#include "Scene3D/scene.h"
#include "noise_2d.h"
#include "noise_3d.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Scene/scene_camera_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "vertex_billboard_hlsl.h"
#include "vertex_cube_hlsl.h"
#include "fragment_angular_hlsl.h"
#include "fragment_billboard_hlsl.h"
#include "fragment_cube_hlsl.h"
#include "fragment_sphere_hlsl.h"

using namespace uicore;

SkyboxPass::SkyboxPass(SceneRender &inout) : inout(inout)
{
}

void SkyboxPass::run()
{
	ScopeTimeFunction();

	setup();

	if (!inout.skybox_texture && !inout.scene->skybox_gradient.empty())
	{
		const auto &colors = inout.scene->skybox_gradient;

		auto pb = PixelBuffer::create(1, colors.size(), tf_rgba32f);
		Vec4f *pixels = pb->data<Vec4f>();

		for (size_t i = 0; i < colors.size(); i++)
		{
			pixels[i] = Vec4f(colors[i].r, colors[i].g, colors[i].b, colors[i].a);
		}

		auto texture = Texture2D::create(inout.gc, pb->size(), tf_rgba32f);
		texture->set_image(inout.gc, pb);
		texture->set_min_filter(filter_linear);
		texture->set_mag_filter(filter_linear);

		inout.skybox_texture = texture;
	}

	inout.gc->set_frame_buffer(inout.fb_self_illumination);

	Size viewport_size = inout.viewport.size();
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());

	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);
	inout.gc->set_rasterizer_state(rasterizer_state);

	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e4f, handed_left, inout.gc->clip_z_range());

	Uniforms cpu_uniforms;
	cpu_uniforms.eye_to_projection = eye_to_projection;
	cpu_uniforms.object_to_eye = Quaternionf::inverse(inout.camera->orientation()).to_matrix();
	uniforms.upload_data(inout.gc, &cpu_uniforms, 1);

	inout.gc->set_depth_range(0.9f, 1.0f);

	if (inout.show_skybox_stars)
	{
		inout.gc->set_program_object(billboard_program);
		inout.gc->set_primitives_array(billboard_prim_array);
		inout.gc->set_uniform_buffer(0, uniforms);
		inout.gc->set_texture(0, star_instance_texture);
		inout.gc->set_texture(1, star_texture);
		inout.gc->draw_primitives_array_instanced(type_triangles, 0, 6, num_star_instances);
		inout.gc->reset_primitives_array();
	}

	inout.gc->set_program_object(cube_program);
	inout.gc->set_primitives_array(cube_prim_array);
	inout.gc->set_uniform_buffer(0, uniforms);
	inout.gc->set_texture(0, inout.skybox_texture);
	inout.gc->draw_primitives_array(type_triangles, 0, 6 * 6);
	inout.gc->reset_primitives_array();

	inout.gc->set_depth_range(0.0f, 1.0f);

	inout.gc->reset_blend_state();
	inout.gc->reset_rasterizer_state();
	inout.gc->reset_depth_stencil_state();
	inout.gc->reset_program_object();
	inout.gc->reset_primitives_elements();
	inout.gc->reset_texture(0);
	inout.gc->reset_texture(1);
	inout.gc->reset_uniform_buffer(0);

	inout.gc->reset_frame_buffer();
}

void SkyboxPass::setup()
{
	if (!cube_program)
	{
		create_clouds();
		create_stars();
		create_programs();
	}

	Size viewport_size = inout.viewport.size();
	if (!blend_state)
	{
		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_src_alpha, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_state = inout.gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = inout.gc->create_depth_stencil_state(depth_stencil_desc);

		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_culled(false);
		rasterizer_state = inout.gc->create_rasterizer_state(rasterizer_desc);

		billboard_prim_array = PrimitivesArray::create(inout.gc);
		billboard_prim_array->set_attributes(0, billboard_positions);

		cube_prim_array = PrimitivesArray::create(inout.gc);
		cube_prim_array->set_attributes(0, cube_positions);
	}
}

void SkyboxPass::create_clouds()
{
	if (!inout.skybox_texture)
		create_cloud_texture();
}

void SkyboxPass::create_cloud_texture()
{
	int width = 512;
	int height = 512;
	int turbulence = 300;
	Color color = Color::mediumslateblue;

	Noise2D noise(width, height);
	auto cloud = PixelBuffer::create(width, height, tf_rgba16);
	Vec4us *cloud_data = cloud->data<Vec4us>();
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int alpha = (int)clamp(noise.turbulence(x, y, (double)turbulence) * 65535.0 - 20000.0, 0.0, 65535.0);
			cloud_data[x+y*512] = Vec4us(color.r * 64, color.g * 64, color.b * 64, alpha);
		}
	}
	auto texture = Texture2D::create(inout.gc, width, height, tf_rgba16, 0);
	texture->set_image(inout.gc, cloud);
	texture->generate_mipmap();
	inout.skybox_texture = texture;
}

void SkyboxPass::create_stars()
{
	create_star_texture();

	auto instances = PixelBuffer::create(num_star_instances, 1, tf_rgba32f);
	Vec4f *instances_data = instances->data<Vec4f>();
	for (size_t i = 0; i < num_star_instances; i++)
	{
		float range = 700.0f;
		do
		{
			instances_data[i] = Vec4f(random(-range, range), random(-range, range), random(-range, range), 1.0f);
		} while (instances_data[i].length3() < range * 0.5f);
	}

	star_instance_texture = Texture2D::create(inout.gc, num_star_instances, 1, tf_rgba32f);
	star_instance_texture->set_image(inout.gc, instances);
}

void SkyboxPass::create_star_texture()
{
	auto star_icon = PixelBuffer::create(512, 512, tf_rgba16);
	Vec4us *star_icon_data = star_icon->data<Vec4us>();
	for (int y = 0; y < 512; y++)
	{
		for (int x = 0; x < 512; x++)
		{
			float s = (x + 0.5f) / 512.0f;
			float t = (y + 0.5f) / 512.0f;
			float distance = 1.0f - (Vec2f(s,t) * 2.0f - 1.0f).length();
			distance *= 1.25f;
			star_icon_data[x + y * 512] = Vec4us(230*256, 230*256, 255*256, clamp((int)(distance * 65535.0f + 0.5f), 0, 65535));
		}
	}
	star_texture = Texture2D::create(inout.gc, 512, 512, tf_rgba16);
	star_texture->set_image(inout.gc, star_icon);
	star_texture->generate_mipmap();
}

void SkyboxPass::create_programs()
{
	uniforms = UniformVector<Uniforms>(inout.gc, 1);
	create_billboard_program();
	create_cube_program();
}

void SkyboxPass::create_billboard_program()
{
	billboard_positions = VertexArrayVector<Vec3f>(inout.gc, cpu_billboard_positions, 6);

	auto vertex_shader = ShaderObject::create(inout.gc, ShaderType::vertex, vertex_billboard_hlsl());
	vertex_shader->compile();

	auto fragment_shader = ShaderObject::create(inout.gc, ShaderType::fragment, fragment_billboard_hlsl());
	fragment_shader->compile();

	billboard_program = ProgramObject::create(inout.gc);
	billboard_program->attach(vertex_shader);
	billboard_program->attach(fragment_shader);
	billboard_program->bind_attribute_location(0, "AttrPosition");
	billboard_program->bind_frag_data_location(0, "FragColor");
	if (!billboard_program->try_link())
		throw Exception(string_format("Link failed: %1", billboard_program->get_info_log()));
	billboard_program->set_uniform1i("InstanceTexture", 0);
	billboard_program->set_uniform1i("ParticleTexture", 1);
	billboard_program->set_uniform1i("ParticleTextureSampler", 1);
	billboard_program->set_uniform_buffer_index("Uniforms", 0);
}

void SkyboxPass::create_cube_program()
{
	cube_positions = VertexArrayVector<Vec3f>(inout.gc, cpu_cube_positions, 36);

	auto vertex_shader = ShaderObject::create(inout.gc, ShaderType::vertex, vertex_cube_hlsl());
	vertex_shader->compile();

	auto fragment_shader = ShaderObject::create(inout.gc, ShaderType::fragment, fragment_sphere_hlsl());
	fragment_shader->compile();

	cube_program = ProgramObject::create(inout.gc);
	cube_program->attach(vertex_shader);
	cube_program->attach(fragment_shader);
	cube_program->bind_attribute_location(0, "AttrPosition");
	cube_program->bind_frag_data_location(0, "FragColor");
	if (!cube_program->try_link())
		throw Exception(string_format("Link failed: %1", cube_program->get_info_log()));
	cube_program->set_uniform1i("SkyboxTexture", 0);
	cube_program->set_uniform1i("SkyboxTextureSampler", 0);
	cube_program->set_uniform_buffer_index("Uniforms", 0);
}

float SkyboxPass::random(float min_value, float max_value)
{
	float v = rand() / (float)RAND_MAX;
	return min_value + v * (max_value - min_value);
}

Vec3f SkyboxPass::cpu_billboard_positions[6] = 
{
	Vec3f(-1.0f, -1.0f, 0.0f),
	Vec3f( 1.0f, -1.0f, 0.0f),
	Vec3f(-1.0f,  1.0f, 0.0f),
	Vec3f(-1.0f,  1.0f, 0.0f),
	Vec3f( 1.0f, -1.0f, 0.0f),
	Vec3f( 1.0f,  1.0f, 0.0f)
};

Vec3f SkyboxPass::cpu_cube_positions[6 * 6] =
{
	Vec3f(-1.0f,  1.0f,  1.0f),
	Vec3f( 1.0f,  1.0f,  1.0f),
	Vec3f( 1.0f, -1.0f,  1.0f),

	Vec3f( 1.0f, -1.0f,  1.0f),
	Vec3f(-1.0f, -1.0f,  1.0f),
	Vec3f(-1.0f,  1.0f,  1.0f),


	Vec3f( 1.0f, -1.0f, -1.0f),
	Vec3f( 1.0f,  1.0f, -1.0f),
	Vec3f(-1.0f,  1.0f, -1.0f),

	Vec3f(-1.0f,  1.0f, -1.0f),
	Vec3f(-1.0f, -1.0f, -1.0f),
	Vec3f( 1.0f, -1.0f, -1.0f),


	Vec3f( 1.0f,  1.0f, -1.0f),
	Vec3f( 1.0f,  1.0f,  1.0f),
	Vec3f(-1.0f,  1.0f,  1.0f),

	Vec3f(-1.0f,  1.0f,  1.0f),
	Vec3f(-1.0f,  1.0f, -1.0f),
	Vec3f( 1.0f,  1.0f, -1.0f),


	Vec3f(-1.0f, -1.0f,  1.0f),
	Vec3f( 1.0f, -1.0f,  1.0f),
	Vec3f( 1.0f, -1.0f, -1.0f),

	Vec3f( 1.0f, -1.0f, -1.0f),
	Vec3f(-1.0f, -1.0f, -1.0f),
	Vec3f(-1.0f, -1.0f,  1.0f),


	Vec3f( 1.0f, -1.0f,  1.0f),
	Vec3f( 1.0f,  1.0f,  1.0f),
	Vec3f( 1.0f,  1.0f, -1.0f),

	Vec3f( 1.0f,  1.0f, -1.0f),
	Vec3f( 1.0f, -1.0f, -1.0f),
	Vec3f( 1.0f, -1.0f,  1.0f),


	Vec3f(-1.0f,  1.0f, -1.0f),
	Vec3f(-1.0f,  1.0f,  1.0f),
	Vec3f(-1.0f, -1.0f,  1.0f),

	Vec3f(-1.0f, -1.0f,  1.0f),
	Vec3f(-1.0f, -1.0f, -1.0f),
	Vec3f(-1.0f,  1.0f, -1.0f)
};
