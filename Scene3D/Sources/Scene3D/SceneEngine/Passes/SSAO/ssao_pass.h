
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class SSAOPass : public ScenePass
{
public:
	SSAOPass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, SceneRender &inout);
	std::string name() const override { return "ssao"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	static float random_value();

	SceneRender &inout;

	uicore::ProgramObjectPtr extract_shader;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::BlendStatePtr blend_state;
};
