
#pragma once

#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class LogAverageLight
{
public:
	LogAverageLight(uicore::GraphicContext &gc, ResourceContainer &inout, int iterations = 8);
	uicore::Texture2D &find_log_average_light(uicore::GraphicContext &gc, uicore::Texture2D &hdr_texture);

private:
	uicore::ProgramObjectPtr compile_and_link(uicore::GraphicContext &gc, const std::string &vertex_source, const std::string &fragment_source);

	// In:
	Resource<uicore::Rect> viewport;

	int iterations;
	uicore::ProgramObjectPtr program0, program1, program2;
	uicore::PrimitivesArray prim_array;
	uicore::VertexArrayVector<uicore::Vec4f> vertices;
	uicore::FrameBuffer fb0, fb1, fb2, fb3;
	uicore::Texture2D texture0, texture1;
	uicore::Texture2D result_texture0, result_texture1;
	int current_index;
	uicore::BlendStatePtr blend_state;
};
