
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_light_impl.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"
#include "icosahedron.h"

class GPUTimer;
class Scene_Impl;

class LightsourceSimplePass : public ScenePass, SceneLightVisitor
{
public:
	LightsourceSimplePass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout);
	~LightsourceSimplePass();

	std::string name() const override { return "light"; }
	void run(const uicore::GraphicContextPtr &gc, Scene_Impl *scene) override;

private:
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &shader_path, const std::string &type);

	void setup(const uicore::GraphicContextPtr &gc);
	void find_lights(const uicore::GraphicContextPtr &gc, Scene_Impl *scene);
	void upload(const uicore::GraphicContextPtr &gc, Scene_Impl *scene);
	void render(const uicore::GraphicContextPtr &gc, GPUTimer &timer);

	// SceneLightVisitor
	void light(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneLight_Impl *light);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;
	Resource<uicore::Texture2DPtr> diffuse_color_gbuffer;
	Resource<uicore::Texture2DPtr> specular_color_gbuffer;
	Resource<uicore::Texture2DPtr> specular_level_gbuffer;
	Resource<uicore::Texture2DPtr> self_illumination_gbuffer;
	Resource<uicore::Texture2DPtr> normal_z_gbuffer;
	Resource<uicore::Texture2DArrayPtr> shadow_maps;
	Resource<uicore::Texture2DPtr> zbuffer;

	// Out:
	Resource<uicore::Texture2DPtr> final_color;

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

	uicore::FrameBufferPtr fb;
	uicore::BlendStatePtr blend_state;

	uicore::DepthStencilStatePtr icosahedron_depth_stencil_state;
	uicore::RasterizerStatePtr icosahedron_rasterizer_state;

	uicore::DepthStencilStatePtr rect_depth_stencil_state;
	uicore::RasterizerStatePtr rect_rasterizer_state;

	uicore::PrimitivesArrayPtr icosahedron_prim_array;
	uicore::PrimitivesArrayPtr rect_prim_array;

	uicore::UniformVector<Uniforms> uniforms;

	std::unique_ptr<Icosahedron> icosahedron;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;

	uicore::PixelBufferPtr light_instance_transfer;
	uicore::Texture1DPtr light_instance_texture;

	uicore::ProgramObjectPtr icosahedron_light_program;
	uicore::ProgramObjectPtr rect_light_program;

	std::vector<SceneLight_Impl *> lights;
};
