
#pragma once

#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class SSAOPass
{
public:
	SSAOPass(uicore::GraphicContext &gc, const std::string &shader_path, ResourceContainer &inout);
	void run(uicore::GraphicContext &gc);

private:
	static float random_value();

	// In:
	Resource<uicore::Texture2D> normal_z_gbuffer;

	// Out:
	Resource<uicore::Texture2D> ssao_contribution;

	GaussianBlur blur;
	uicore::ProgramObjectPtr extract_shader;
	uicore::FrameBuffer fb;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArray rect_primarray;
	uicore::BlendStatePtr blend_state;
};
