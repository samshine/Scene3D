
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/SceneCache/shadow_maps.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/Model/model_mesh_visitor.h"
#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_light_impl.h"

class Scene_Impl;
class VSMShadowMapPassLightData;

class VSMShadowMapPass : public ScenePass, ModelMeshVisitor, SceneLightVisitor
{
public:
	VSMShadowMapPass(const uicore::GraphicContextPtr &gc, ResourceContainer &inout);
	std::string name() const override { return "shadow"; }
	void run(const uicore::GraphicContextPtr &gc, Scene_Impl *scene) override;

private:
	void find_lights(Scene_Impl *scene);
	void assign_shadow_map_indexes();
	void render_maps(Scene_Impl *scene);
	void blur_maps();

	// ModelMeshVisitor
	void render(const uicore::GraphicContextPtr &gc, ModelLOD *model_lod, int num_instances);

	// SceneLightVisitor
	void light(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneLight_Impl *light);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;

	std::vector<SceneLight_Impl *> lights;
	int round_robin;
	std::vector<size_t> blur_indexes;

	ShadowMaps maps;
	GaussianBlur blur;
	Scene_Impl *scene;
	uicore::GraphicContextPtr gc;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;

	friend class VSMShadowMapPassLightData;
};

class VSMShadowMapPassLightData
{
public:
	VSMShadowMapPassLightData(VSMShadowMapPass *pass, SceneLight_Impl *light) : pass(pass), light(light), shadow_map(&pass->maps) { }

	VSMShadowMapPass *pass;
	SceneLight_Impl *light;

	uicore::Mat4f eye_to_projection;
	uicore::Mat4f world_to_eye;
	uicore::Mat4f world_to_shadow_projection;
	ShadowMapEntry shadow_map;
};
