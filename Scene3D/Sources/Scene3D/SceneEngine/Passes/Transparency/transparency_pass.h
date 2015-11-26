
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/scene_render.h"

class SceneImpl;

class TransparencyPass : public ScenePass
{
public:
	TransparencyPass(SceneRender &inout);
	std::string name() const override { return "transparency"; }
	void run() override;

private:
	void setup();

	SceneRender &inout;

	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
