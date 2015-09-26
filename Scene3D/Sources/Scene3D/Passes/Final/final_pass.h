
#pragma once

#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class FinalPass
{
public:
	FinalPass(uicore::GraphicContext &gc, const std::string &shader_path, ResourceContainer &inout);
	void run(uicore::GraphicContext &gc);

private:
	// In:
	Resource<uicore::FrameBuffer> viewport_fb;
	Resource<uicore::Rect> viewport;
	Resource<uicore::Texture2D> final_color;
	Resource<uicore::Texture2D> bloom_blur_texture;
	Resource<uicore::Texture2D> ambient_occlusion;

	uicore::ProgramObject present_shader;
	uicore::VertexArrayVector<uicore::Vec4f> rect_positions;
	uicore::PrimitivesArray rect_primarray;
	uicore::RasterizerState rasterizer_state;
};
