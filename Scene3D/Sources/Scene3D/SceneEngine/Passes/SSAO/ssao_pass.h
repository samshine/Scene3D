
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class SSAOPass : public ScenePass
{
public:
	SSAOPass(const uicore::GraphicContextPtr &gc, SceneRender &inout);
	std::string name() const override { return "ssao"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	struct UniformBuffer
	{
		float f;
		float f_div_aspect;
		float padding0, padding1;
		uicore::Vec4f sampledata[160];
	};

	static float random_value();

	SceneRender &inout;

	uicore::ProgramObjectPtr extract_shader;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::BlendStatePtr blend_state;
	uicore::UniformVector<UniformBuffer> uniforms;
};
