
#include "precomp.h"
#include "skybox_pass.h"
#include "Scene3D/scene.h"
#include "noise_2d.h"
#include "noise_3d.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

SkyboxPass::SkyboxPass(const std::string &shader_path, ResourceContainer &inout) : shader_path(shader_path)
{
	viewport = inout.get<Rect>("Viewport");
	field_of_view = inout.get<float>("FieldOfView");
	world_to_eye = inout.get<Mat4f>("WorldToEye");

	diffuse_color_gbuffer = inout.get<Texture2DPtr>("DiffuseColorGBuffer");
	specular_color_gbuffer = inout.get<Texture2DPtr>("SpecularColorGBuffer");
	specular_level_gbuffer = inout.get<Texture2DPtr>("SpecularLevelGBuffer");
	self_illumination_gbuffer = inout.get<Texture2DPtr>("SelfIlluminationGBuffer");
	normal_z_gbuffer = inout.get<Texture2DPtr>("NormalZGBuffer");
	zbuffer = inout.get<Texture2DPtr>("ZBuffer");

	cloud_texture = inout.get<Texture2DPtr>("SkyboxTexture");
	show_stars = inout.get<bool>("ShowSkyboxStars");
}

void SkyboxPass::run(const GraphicContextPtr &gc, Scene_Impl *scene)
{
	setup(gc);

	gc->set_frame_buffer(fb);

	Size viewport_size = viewport->size();
	gc->set_viewport(viewport_size, gc->texture_image_y_axis());

	gc->set_depth_stencil_state(depth_stencil_state);
	gc->set_blend_state(blend_state);
	gc->set_rasterizer_state(rasterizer_state);

	Mat4f eye_to_projection = Mat4f::perspective(field_of_view.get(), viewport_size.width/(float)viewport_size.height, 0.1f, 1.e4f, handed_left, gc->clip_z_range());

	Uniforms cpu_uniforms;
	cpu_uniforms.eye_to_projection = eye_to_projection;
	cpu_uniforms.object_to_eye = Quaternionf::inverse(scene->get_camera()->orientation()).to_matrix();
	uniforms.upload_data(gc, &cpu_uniforms, 1);

	gc->set_depth_range(0.9f, 1.0f);

	if (show_stars.get())
	{
		gc->set_program_object(billboard_program);
		gc->set_primitives_array(billboard_prim_array);
		gc->set_uniform_buffer(0, uniforms);
		gc->set_texture(0, star_instance_texture);
		gc->set_texture(1, star_texture);
		gc->draw_primitives_array_instanced(type_triangles, 0, 6, num_star_instances);
		gc->reset_primitives_array();
	}

	gc->set_program_object(cube_program);
	gc->set_primitives_array(cube_prim_array);
	gc->set_uniform_buffer(0, uniforms);
	gc->set_texture(0, cloud_texture.get());
	gc->draw_primitives_array(type_triangles, 0, 6*6);
	gc->reset_primitives_array();

	gc->set_depth_range(0.0f, 1.0f);

	gc->reset_blend_state();
	gc->reset_rasterizer_state();
	gc->reset_depth_stencil_state();
	gc->reset_program_object();
	gc->reset_primitives_elements();
	gc->reset_texture(0);
	gc->reset_texture(1);
	gc->reset_uniform_buffer(0);

	gc->reset_frame_buffer();
}

void SkyboxPass::setup(const GraphicContextPtr &gc)
{
	if (!cube_program)
	{
		create_clouds(gc);
		create_stars(gc);
		create_programs(gc);
	}

	Size viewport_size = viewport->size();
	if (!fb || !gc->is_frame_buffer_owner(fb) || zbuffer.updated())
	{
		fb = FrameBuffer::create(gc);
		fb->attach_color(0, self_illumination_gbuffer.get());
/*
		fb->attach_color(0, diffuse_color_gbuffer.get());
		fb->attach_color(1, specular_color_gbuffer.get());
		fb->attach_color(2, specular_level_gbuffer.get());
		fb->attach_color(3, self_illumination_gbuffer.get());
		fb->attach_color(4, normal_z_gbuffer.get());
*/
		fb->attach_depth(zbuffer.get());

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_src_alpha, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_state = gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = gc->create_depth_stencil_state(depth_stencil_desc);

		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_culled(false);
		rasterizer_state = gc->create_rasterizer_state(rasterizer_desc);

		billboard_prim_array = PrimitivesArray::create(gc);
		billboard_prim_array->set_attributes(0, billboard_positions);

		cube_prim_array = PrimitivesArray::create(gc);
		cube_prim_array->set_attributes(0, cube_positions);
	}
}

void SkyboxPass::create_clouds(const GraphicContextPtr &gc)
{
	if (!cloud_texture.get())
		create_cloud_texture(gc);
}

void SkyboxPass::create_cloud_texture(const GraphicContextPtr &gc)
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
	auto texture = Texture2D::create(gc, width, height, tf_rgba16, 0);
	texture->set_image(gc, cloud);
	texture->generate_mipmap();
	cloud_texture.set(texture);
}

void SkyboxPass::create_stars(const GraphicContextPtr &gc)
{
	create_star_texture(gc);

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

	star_instance_texture = Texture2D::create(gc, num_star_instances, 1, tf_rgba32f);
	star_instance_texture->set_image(gc, instances);
}

void SkyboxPass::create_star_texture(const GraphicContextPtr &gc)
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
	star_texture = Texture2D::create(gc, 512, 512, tf_rgba16);
	star_texture->set_image(gc, star_icon);
	star_texture->generate_mipmap();
}

void SkyboxPass::create_programs(const GraphicContextPtr &gc)
{
	uniforms = UniformVector<Uniforms>(gc, 1);
	create_billboard_program(gc);
	create_cube_program(gc);
}

void SkyboxPass::create_billboard_program(const GraphicContextPtr &gc)
{
	billboard_positions = VertexArrayVector<Vec3f>(gc, cpu_billboard_positions, 6);

	billboard_program = ProgramObject::load(gc, PathHelp::combine(shader_path, "Skybox/vertex_billboard.hlsl"), PathHelp::combine(shader_path, "Skybox/fragment_billboard.hlsl"));
	billboard_program->bind_attribute_location(0, "AttrPosition");
	billboard_program->bind_frag_data_location(0, "FragColor");
	if (!billboard_program->try_link())
		throw Exception(string_format("Link failed: %1", billboard_program->get_info_log()));
	billboard_program->set_uniform1i("InstanceTexture", 0);
	billboard_program->set_uniform1i("ParticleTexture", 1);
	billboard_program->set_uniform1i("ParticleTextureSampler", 1);
	billboard_program->set_uniform_buffer_index("Uniforms", 0);
}

void SkyboxPass::create_cube_program(const GraphicContextPtr &gc)
{
	cube_positions = VertexArrayVector<Vec3f>(gc, cpu_cube_positions, 36);

	cube_program = ProgramObject::load(gc, PathHelp::combine(shader_path, "Skybox/vertex_cube.hlsl"), PathHelp::combine(shader_path, "Skybox/fragment_sphere.hlsl"));
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
