
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class BloomPass : public ScenePass
{
public:
	BloomPass(SceneRender &inout);
	std::string name() const override { return "bloom"; }
	void run() override;

private:
	SceneRender &inout;

	uicore::ProgramObjectPtr extract_shader;
	uicore::ProgramObjectPtr combine_shader;
	uicore::BlendStatePtr add_blend_state;
};
