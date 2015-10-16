
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/Passes/ZMinMax/z_minmax.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_light_impl.h"

class GPUTimer;
class SceneImpl;

class LightsourcePass : public ScenePass
{
public:
	LightsourcePass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout);
	~LightsourcePass();

	std::string name() const override { return "light"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void find_lights(const uicore::GraphicContextPtr &gc, SceneImpl *scene);
	void upload(const uicore::GraphicContextPtr &gc);
	void render(const uicore::GraphicContextPtr &gc, GPUTimer &timer);
	void update_buffers(const uicore::GraphicContextPtr &gc);
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &compute_filename, const std::string &defines = std::string());

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

	// Out:
	Resource<uicore::Texture2DPtr> final_color;

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
	uicore::StagingVector<GPULight> transfer_lights;
	uicore::StorageVector<unsigned int> compute_visible_lights;

	uicore::ProgramObjectPtr cull_tiles_program;
	uicore::ProgramObjectPtr render_tiles_program;

	std::vector<SceneLightImpl *> lights;

	int tile_size;
	int num_tiles_x;
	int num_tiles_y;

	ZMinMax zminmax;
};
