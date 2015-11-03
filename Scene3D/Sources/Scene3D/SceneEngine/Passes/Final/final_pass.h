
#pragma once

#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/Passes/scene_pass.h"

class FinalPass : public ScenePass
{
public:
	FinalPass(const uicore::GraphicContextPtr &gc, SceneRender &inout);
	std::string name() const override { return "final"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	SceneRender &inout;

	uicore::ProgramObjectPtr present_shader;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::RasterizerStatePtr rasterizer_state;
};
