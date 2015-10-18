
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class SceneImpl;
class ModelLOD;

class GBufferPass : public ScenePass
{
public:
	GBufferPass(ResourceContainer &inout);
	std::string name() const override { return "gbuffer"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void setup_gbuffer(const uicore::GraphicContextPtr &gc);

	ResourceContainer &inout;

	uicore::GraphicContextPtr gc;
	SceneImpl *scene;
	uicore::BlendStatePtr blend_state;
	uicore::BlendStatePtr early_z_blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;

	struct RenderEntry
	{
		RenderEntry(ModelLOD *model_lod, int num_instances) : model_lod(model_lod), num_instances(num_instances) { }

		ModelLOD *model_lod;
		int num_instances;
	};
	std::vector<RenderEntry> render_list;
};
