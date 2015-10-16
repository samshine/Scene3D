
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/SceneCache/resource_container.h"

class SceneImpl;

class TransparencyPass : public ScenePass
{
public:
	TransparencyPass(ResourceContainer &inout);
	std::string name() const override { return "transparency"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void setup(const uicore::GraphicContextPtr &gc);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;
	Resource<uicore::Texture2DPtr> zbuffer;

	// InOut:
	Resource<uicore::Texture2DPtr> final_color;

	SceneImpl *scene;
	uicore::FrameBufferPtr fb_transparency;
	uicore::GraphicContextPtr gc;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
