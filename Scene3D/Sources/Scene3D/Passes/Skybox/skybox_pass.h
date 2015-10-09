
#pragma once

#include "Scene3D/SceneCache/resource_container.h"

class Scene_Impl;

class SkyboxPass
{
public:
	SkyboxPass(const std::string &shader_path, ResourceContainer &inout);
	void run(const uicore::GraphicContextPtr &gc, Scene_Impl *scene);

	void show_skybox_stars(bool enable);
	void set_skybox_texture(uicore::Texture2DPtr texture);

private:
	void setup(const uicore::GraphicContextPtr &gc);
	void create_clouds(const uicore::GraphicContextPtr &gc);
	void create_cloud_texture(const uicore::GraphicContextPtr &gc);
	void create_stars(const uicore::GraphicContextPtr &gc);
	void create_star_texture(const uicore::GraphicContextPtr &gc);
	void create_programs(const uicore::GraphicContextPtr &gc);
	void create_billboard_program(const uicore::GraphicContextPtr &gc);
	void create_cube_program(const uicore::GraphicContextPtr &gc);
	static float random(float min_value, float max_value);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;

	// InOut:
	Resource<uicore::Texture2DPtr> diffuse_color_gbuffer;
	Resource<uicore::Texture2DPtr> specular_color_gbuffer;
	Resource<uicore::Texture2DPtr> specular_level_gbuffer;
	Resource<uicore::Texture2DPtr> self_illumination_gbuffer;
	Resource<uicore::Texture2DPtr> normal_z_gbuffer;
	Resource<uicore::Texture2DPtr> zbuffer;

	std::string shader_path;

	struct Uniforms
	{
		uicore::Mat4f object_to_eye;
		uicore::Mat4f eye_to_projection;
	};

	uicore::FrameBufferPtr fb;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;

	uicore::UniformVector<Uniforms> uniforms;

	uicore::ProgramObjectPtr billboard_program;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	uicore::PrimitivesArrayPtr billboard_prim_array;

	uicore::ProgramObjectPtr cube_program;
	uicore::VertexArrayVector<uicore::Vec3f> cube_positions;
	uicore::PrimitivesArrayPtr cube_prim_array;

	static uicore::Vec3f cpu_billboard_positions[6];
	static uicore::Vec3f cpu_cube_positions[6 * 6];

	uicore::Texture2DPtr cloud_texture;

	uicore::Texture2DPtr star_texture;
	uicore::Texture2DPtr star_instance_texture;
	static const int num_star_instances = 1024;

	bool show_stars;
};
