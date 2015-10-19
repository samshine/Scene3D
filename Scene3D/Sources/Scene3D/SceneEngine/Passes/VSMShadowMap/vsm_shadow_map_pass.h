
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/shadow_maps.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/Scene/scene_light_impl.h"

class SceneImpl;
class SceneRender;
class VSMShadowMapPassLightData;

class VSMShadowMapPass : public ScenePass
{
public:
	VSMShadowMapPass(const uicore::GraphicContextPtr &gc, SceneRender &inout);
	std::string name() const override { return "shadow"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void find_lights(SceneImpl *scene);
	void assign_shadow_map_indexes();
	void render_maps(SceneImpl *scene);
	void blur_maps();

	SceneRender &inout;
	std::vector<SceneLightImpl *> lights;
	int round_robin;
	std::vector<size_t> blur_indexes;

	ShadowMaps maps;
	SceneImpl *scene;
	uicore::GraphicContextPtr gc;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;

	friend class VSMShadowMapPassLightData;
};

class VSMShadowMapPassLightData
{
public:
	VSMShadowMapPassLightData(VSMShadowMapPass *pass, SceneLightImpl *light) : pass(pass), light(light), shadow_map(&pass->maps) { }

	VSMShadowMapPass *pass;
	SceneLightImpl *light;

	uicore::Mat4f eye_to_projection;
	uicore::Mat4f world_to_eye;
	uicore::Mat4f world_to_shadow_projection;
	ShadowMapEntry shadow_map;
};
