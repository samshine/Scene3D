
#pragma once

#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/Passes/scene_pass.h"

class FinalPass : public ScenePass
{
public:
	FinalPass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout);
	std::string name() const override { return "final"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	ResourceContainer &inout;

	uicore::ProgramObjectPtr present_shader;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::RasterizerStatePtr rasterizer_state;
};
