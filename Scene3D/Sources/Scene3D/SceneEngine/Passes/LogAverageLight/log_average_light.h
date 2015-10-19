
#pragma once

#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class LogAverageLight
{
public:
	LogAverageLight(const uicore::GraphicContextPtr &gc, SceneRender &inout, int iterations = 8);
	uicore::Texture2DPtr &find_log_average_light(const uicore::GraphicContextPtr &gc, uicore::Texture2DPtr &hdr_texture);

private:
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &vertex_source, const std::string &fragment_source);

	SceneRender &inout;

	int iterations;
	uicore::ProgramObjectPtr program0, program1, program2;
	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec4f> vertices;
	uicore::FrameBufferPtr fb0, fb1, fb2, fb3;
	uicore::Texture2DPtr texture0, texture1;
	uicore::Texture2DPtr result_texture0, result_texture1;
	int current_index;
	uicore::BlendStatePtr blend_state;
};
