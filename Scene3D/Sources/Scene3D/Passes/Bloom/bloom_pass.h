
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class BloomPass : public ScenePass
{
public:
	BloomPass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout);
	std::string name() const override { return "bloom"; }
	void run(const uicore::GraphicContextPtr &gc, Scene_Impl *scene) override;

private:
	void setup_bloom_extract(const uicore::GraphicContextPtr &gc);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<uicore::Texture2DPtr> final_color;

	// Out:
	Resource<uicore::Texture2DPtr> bloom_contribution;

	GaussianBlur bloom_blur;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::ProgramObjectPtr bloom_shader;
	uicore::FrameBufferPtr fb_bloom_extract;
	uicore::BlendStatePtr blend_state;
};
