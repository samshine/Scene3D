
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/Passes/ZMinMax/z_minmax.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/Scene/scene_light_impl.h"

class GPUTimer;
class SceneRender;
class SceneImpl;

struct LightsourceUniforms
{
	float rcp_f;
	float rcp_f_div_aspect;
	uicore::Vec2f two_rcp_viewport_size;
	unsigned int num_lights;
	unsigned int num_tiles_x;
	unsigned int num_tiles_y;
	unsigned int padding;
	uicore::Vec4f scene_ambience;
};

struct LightsourceGPULight
{
	uicore::Vec4f position;
	uicore::Vec4f color;
	uicore::Vec4f range; // pow(attenuation_end, 2), pow(attenation_start, 2), 1/pow(attenuation_end-attenuation_start, 2), hotspot
	uicore::Vec4f spot_x;
	uicore::Vec4f spot_y;
	uicore::Vec4f spot_z;
};

class LightsourcePass : public ScenePass
{
public:
	LightsourcePass(SceneRender &inout);
	~LightsourcePass();

	std::string name() const override { return "light"; }
	void run() override;

private:
	void find_lights();
	void upload();
	void render();
	void update_buffers();
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &program_name, const std::string &source, const std::string &defines = std::string());

	SceneRender &inout;

	static const int max_lights = 1023;
	static const int light_slots_per_tile = 128;

	uicore::ProgramObjectPtr cull_tiles_program;
	uicore::ProgramObjectPtr render_tiles_program;

	std::vector<SceneLightImpl *> lights;

	const int tile_size = 16;
	int num_tiles_x = 0;
	int num_tiles_y = 0;
};
