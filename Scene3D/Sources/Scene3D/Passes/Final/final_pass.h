
#pragma once

#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class FinalPass
{
public:
	FinalPass(const uicore::GraphicContextPtr &gc, const std::string &shader_path, ResourceContainer &inout);
	void run(const uicore::GraphicContextPtr &gc);

private:
	// In:
	Resource<uicore::FrameBufferPtr> viewport_fb;
	Resource<uicore::Rect> viewport;
	Resource<uicore::Texture2DPtr> final_color;
	Resource<uicore::Texture2DPtr> bloom_blur_texture;
	Resource<uicore::Texture2DPtr> ambient_occlusion;

	uicore::ProgramObjectPtr present_shader;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArrayPtr rect_primarray;
	uicore::RasterizerStatePtr rasterizer_state;
};
