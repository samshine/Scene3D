
#pragma once

#include "Scene3D/SceneCache/shadow_maps.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/Model/model_mesh_visitor.h"
#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_light_impl.h"

class Scene_Impl;
class VSMShadowMapPassLightData;

class VSMShadowMapPass : ModelMeshVisitor, SceneLightVisitor
{
public:
	VSMShadowMapPass(uicore::GraphicContext &gc, ResourceContainer &inout);
	void run(uicore::GraphicContext &gc, Scene_Impl *scene);

private:
	void find_lights(Scene_Impl *scene);
	void assign_shadow_map_indexes();
	void render_maps(Scene_Impl *scene);
	void blur_maps();

	// ModelMeshVisitor
	void render(uicore::GraphicContext &gc, ModelLOD *model_lod, int num_instances);

	// SceneLightVisitor
	void light(uicore::GraphicContext &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneLight_Impl *light);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;

	std::vector<SceneLight_Impl *> lights;
	int round_robin;
	std::vector<size_t> blur_indexes;

	ShadowMaps maps;
	GaussianBlur blur;
	uicore::GraphicContext gc;
	uicore::BlendState blend_state;
	uicore::DepthStencilState depth_stencil_state;

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
