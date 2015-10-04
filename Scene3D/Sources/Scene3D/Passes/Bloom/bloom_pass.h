
#pragma once

#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class BloomPass
{
public:
	BloomPass(uicore::GraphicContext &gc, const std::string &shader_path, ResourceContainer &inout);
	void run(uicore::GraphicContext &gc);

private:
	void setup_bloom_extract(uicore::GraphicContext &gc);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<uicore::Texture2D> final_color;

	// Out:
	Resource<uicore::Texture2D> bloom_contribution;

	GaussianBlur bloom_blur;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArray rect_primarray;
	uicore::ProgramObjectPtr bloom_shader;
	uicore::FrameBuffer fb_bloom_extract;
	uicore::BlendStatePtr blend_state;
};
