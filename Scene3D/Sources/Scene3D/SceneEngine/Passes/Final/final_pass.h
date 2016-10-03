
#pragma once

#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/Passes/scene_pass.h"

class FinalPass : public ScenePass
{
public:
	FinalPass(SceneRender &inout);
	std::string name() const override { return "final"; }
	void run() override;

private:
	SceneRender &inout;

	uicore::ProgramObjectPtr present_shader;
};
