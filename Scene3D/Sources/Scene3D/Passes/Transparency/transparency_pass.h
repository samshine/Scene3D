
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/SceneCache/resource_container.h"

class SceneImpl;

class TransparencyPass : public ScenePass
{
public:
	TransparencyPass(ResourceContainer &inout);
	std::string name() const override { return "transparency"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void setup(const uicore::GraphicContextPtr &gc);

	ResourceContainer &inout;

	SceneImpl *scene;
	uicore::GraphicContextPtr gc;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
