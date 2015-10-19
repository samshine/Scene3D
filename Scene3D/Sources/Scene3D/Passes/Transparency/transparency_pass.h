
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/scene_render.h"

class SceneImpl;

class TransparencyPass : public ScenePass
{
public:
	TransparencyPass(SceneRender &inout);
	std::string name() const override { return "transparency"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void setup(const uicore::GraphicContextPtr &gc);

	SceneRender &inout;

	SceneImpl *scene;
	uicore::GraphicContextPtr gc;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
