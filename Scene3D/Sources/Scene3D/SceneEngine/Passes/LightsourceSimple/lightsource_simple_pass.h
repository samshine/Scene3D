
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/Scene/scene_light_impl.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"
#include "icosahedron.h"

class GPUTimer;
class SceneImpl;

class LightsourceSimplePass : public ScenePass
{
public:
	LightsourceSimplePass(const uicore::GraphicContextPtr &gc, SceneRender &inout);
	~LightsourceSimplePass();

	std::string name() const override { return "light"; }
	void run() override;

private:
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &vertex, const std::string &fragment, const std::string &defines = std::string());

	void setup();
	void find_lights();
	void upload();
	void render();

	SceneRender &inout;

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

	std::vector<SceneLightImpl *> lights;
};
