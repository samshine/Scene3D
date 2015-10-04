
#pragma once

#include "Scene3D/scene_light.h"
#include "Scene3D/scene_light_impl.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"
#include "icosahedron.h"

class GPUTimer;
class Scene_Impl;

class LightsourceSimplePass : SceneLightVisitor
{
public:
	LightsourceSimplePass(uicore::GraphicContext &gc, const std::string &shader_path, ResourceContainer &inout);
	~LightsourceSimplePass();

	void run(uicore::GraphicContext &gc, Scene_Impl *scene);

private:
	uicore::ProgramObjectPtr compile_and_link(uicore::GraphicContext &gc, const std::string &shader_path, const std::string &type);

	void setup(uicore::GraphicContext &gc);
	void find_lights(uicore::GraphicContext &gc, Scene_Impl *scene);
	void upload(uicore::GraphicContext &gc, Scene_Impl *scene);
	void render(uicore::GraphicContext &gc, GPUTimer &timer);

	// SceneLightVisitor
	void light(uicore::GraphicContext &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneLight_Impl *light);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;
	Resource<uicore::Texture2D> diffuse_color_gbuffer;
	Resource<uicore::Texture2D> specular_color_gbuffer;
	Resource<uicore::Texture2D> specular_level_gbuffer;
	Resource<uicore::Texture2D> self_illumination_gbuffer;
	Resource<uicore::Texture2D> normal_z_gbuffer;
	Resource<uicore::Texture2DArray> shadow_maps;
	Resource<uicore::Texture2D> zbuffer;

	// Out:
	Resource<uicore::Texture2D> final_color;

	static const int max_lights = 1023;
	static const int vectors_per_light = 6;

	struct Uniforms
	{
		uicore::Mat4f object_to_eye;
		uicore::Mat4f eye_to_projection;
		float rcp_f;
		float rcp_f_div_aspect;
		uicore::Vec2f two_rcp_viewport_size;
	};

	uicore::FrameBuffer fb;
	uicore::BlendStatePtr blend_state;

	uicore::DepthStencilStatePtr icosahedron_depth_stencil_state;
	uicore::RasterizerStatePtr icosahedron_rasterizer_state;

	uicore::DepthStencilStatePtr rect_depth_stencil_state;
	uicore::RasterizerStatePtr rect_rasterizer_state;

	uicore::PrimitivesArray icosahedron_prim_array;
	uicore::PrimitivesArray rect_prim_array;

	uicore::UniformVector<Uniforms> uniforms;

	std::unique_ptr<Icosahedron> icosahedron;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;

	uicore::PixelBuffer light_instance_transfer;
	uicore::Texture1D light_instance_texture;

	uicore::ProgramObjectPtr icosahedron_light_program;
	uicore::ProgramObjectPtr rect_light_program;

	std::vector<SceneLight_Impl *> lights;
};
