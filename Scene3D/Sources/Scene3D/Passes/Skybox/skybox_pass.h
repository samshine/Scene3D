
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/scene_render.h"

class SceneImpl;

class SkyboxPass : public ScenePass
{
public:
	SkyboxPass(const std::string &shader_path, SceneRender &inout);
	std::string name() const override { return "skybox"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

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

	std::string shader_path;
	SceneRender &inout;

	struct Uniforms
	{
		uicore::Mat4f object_to_eye;
		uicore::Mat4f eye_to_projection;
	};

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

	uicore::Texture2DPtr star_texture;
	uicore::Texture2DPtr star_instance_texture;
	static const int num_star_instances = 1024;
};
