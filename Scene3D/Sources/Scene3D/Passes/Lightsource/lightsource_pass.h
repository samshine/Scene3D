
#pragma once

#include "Scene3D/Passes/ZMinMax/z_minmax.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_light_impl.h"

class GPUTimer;
class Scene_Impl;

class LightsourcePass : SceneLightVisitor
{
public:
	LightsourcePass(uicore::GraphicContext &gc, const std::string &shader_path, ResourceContainer &inout);
	~LightsourcePass();

	void run(uicore::GraphicContext &gc, Scene_Impl *scene);

private:
	void find_lights(uicore::GraphicContext &gc, Scene_Impl *scene);
	void upload(uicore::GraphicContext &gc);
	void render(uicore::GraphicContext &gc, GPUTimer &timer);
	void update_buffers(uicore::GraphicContext &gc);
	uicore::ProgramObject compile_and_link(uicore::GraphicContext &gc, const std::string &compute_filename, const std::string &defines = std::string());

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

	// Out:
	Resource<uicore::Texture2D> final_color;

	static const int max_lights = 1023;
	static const int light_slots_per_tile = 128;

	struct GPULight
	{
		uicore::Vec4f position;
		uicore::Vec4f color;
		uicore::Vec4f range; // pow(attenuation_end, 2), pow(attenation_start, 2), 1/pow(attenuation_end-attenuation_start, 2), hotspot
		uicore::Vec4f spot_x;
		uicore::Vec4f spot_y;
		uicore::Vec4f spot_z;
	};

	struct Uniforms
	{
		float rcp_f;
		float rcp_f_div_aspect;
		uicore::Vec2f two_rcp_viewport_size;
		unsigned int num_lights;
		unsigned int num_tiles_x;
		unsigned int num_tiles_y;
		unsigned int padding;
	};

	uicore::UniformVector<Uniforms> compute_uniforms;
	uicore::StorageVector<GPULight> compute_lights;
	uicore::TransferVector<GPULight> transfer_lights;
	uicore::StorageVector<unsigned int> compute_visible_lights;

	uicore::ProgramObject cull_tiles_program;
	uicore::ProgramObject render_tiles_program;

	std::vector<SceneLight_Impl *> lights;

	int tile_size;
	int num_tiles_x;
	int num_tiles_y;

	ZMinMax zminmax;
};
