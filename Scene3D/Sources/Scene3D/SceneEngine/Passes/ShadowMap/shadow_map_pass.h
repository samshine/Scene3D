
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/Scene/scene_light_impl.h"

class SceneImpl;
class SceneRender;

class ShadowMapPass : public ScenePass
{
public:
	ShadowMapPass(SceneRender &inout);
	std::string name() const override { return "shadow"; }
	void run() override;

private:
	void render_map(ShadowMapLight &slot);

	SceneRender &inout;

	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
