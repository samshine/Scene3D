
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/scene_render.h"

class SceneImpl;
class SceneRender;

class DecalsPass : public ScenePass
{
public:
	DecalsPass(SceneRender &inout);
	std::string name() const override { return "decals"; }
	void run() override;

private:
	SceneRender &inout;
};
