
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class BloomPass : public ScenePass
{
public:
	BloomPass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, SceneRender &inout);
	std::string name() const override { return "bloom"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	SceneRender &inout;

	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::ProgramObjectPtr bloom_shader;
	uicore::BlendStatePtr blend_state;
};
