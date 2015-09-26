
#pragma once

#include "Scene3D/SceneCache/resource_container.h"

class Scene_Impl;

class SkyboxPass
{
public:
	SkyboxPass(const std::string &shader_path, ResourceContainer &inout);
	void run(uicore::GraphicContext &gc, Scene_Impl *scene);

	void show_skybox_stars(bool enable);
	void set_skybox_texture(uicore::Texture2D texture);

private:
	void setup(uicore::GraphicContext &gc);
	void create_clouds(uicore::GraphicContext &gc);
	void create_cloud_texture(uicore::GraphicContext &gc);
	void create_stars(uicore::GraphicContext &gc);
	void create_star_texture(uicore::GraphicContext &gc);
	void create_programs(uicore::GraphicContext &gc);
	void create_billboard_program(uicore::GraphicContext &gc);
	void create_cube_program(uicore::GraphicContext &gc);
	static float random(float min_value, float max_value);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;

	// InOut:
	Resource<uicore::Texture2D> diffuse_color_gbuffer;
	Resource<uicore::Texture2D> specular_color_gbuffer;
	Resource<uicore::Texture2D> specular_level_gbuffer;
	Resource<uicore::Texture2D> self_illumination_gbuffer;
	Resource<uicore::Texture2D> normal_z_gbuffer;
	Resource<uicore::Texture2D> zbuffer;

	std::string shader_path;

	struct Uniforms
	{
		uicore::Mat4f object_to_eye;
		uicore::Mat4f eye_to_projection;
	};

	uicore::FrameBuffer fb;
	uicore::BlendState blend_state;
	uicore::DepthStencilState depth_stencil_state;
	uicore::RasterizerState rasterizer_state;

	uicore::UniformVector<Uniforms> uniforms;

	uicore::ProgramObject billboard_program;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	uicore::PrimitivesArray billboard_prim_array;

	uicore::ProgramObject cube_program;
	uicore::VertexArrayVector<uicore::Vec3f> cube_positions;
	uicore::PrimitivesArray cube_prim_array;

	static uicore::Vec3f cpu_billboard_positions[6];
	static uicore::Vec3f cpu_cube_positions[6 * 6];

	uicore::Texture2D cloud_texture;

	uicore::Texture2D star_texture;
	uicore::Texture2D star_instance_texture;
	static const int num_star_instances = 1024;

	bool show_stars;
};
