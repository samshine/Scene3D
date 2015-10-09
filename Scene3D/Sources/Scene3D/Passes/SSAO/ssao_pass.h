
#pragma once

#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class SSAOPass
{
public:
	SSAOPass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout);
	void run(const uicore::GraphicContextPtr &gc);

private:
	static float random_value();

	// In:
	Resource<uicore::Texture2DPtr> normal_z_gbuffer;

	// Out:
	Resource<uicore::Texture2DPtr> ssao_contribution;

	GaussianBlur blur;
	uicore::ProgramObjectPtr extract_shader;
	uicore::FrameBufferPtr fb;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::BlendStatePtr blend_state;
};
