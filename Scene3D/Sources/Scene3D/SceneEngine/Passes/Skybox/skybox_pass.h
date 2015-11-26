
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/scene_render.h"

class SceneImpl;

class SkyboxPass : public ScenePass
{
public:
	SkyboxPass(SceneRender &inout);
	std::string name() const override { return "skybox"; }
	void run() override;

private:
	void setup();
	void create_clouds();
	void create_cloud_texture();
	void create_stars();
	void create_star_texture();
	void create_programs();
	void create_billboard_program();
	void create_cube_program();
	static float random(float min_value, float max_value);

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
