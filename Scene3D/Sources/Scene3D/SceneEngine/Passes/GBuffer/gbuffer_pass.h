
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class ModelLOD;

class GBufferPass : public ScenePass
{
public:
	GBufferPass(SceneRender &inout);
	std::string name() const override { return "gbuffer"; }
	void run() override;

private:
	void setup_gbuffer();

	SceneRender &inout;

	uicore::BlendStatePtr blend_state;
	uicore::BlendStatePtr early_z_blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
