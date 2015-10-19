
#pragma once

#include "Scene3D/SceneEngine/resource.h"

class ZMinMax
{
public:
	ZMinMax(int tile_size = 16);
	void minmax(const uicore::GraphicContextPtr &gc);

	// In:
	uicore::Rect viewport;
	uicore::Texture2DPtr normal_z;

	// Out:
	uicore::Texture2DPtr result;

private:
	void update_buffers(const uicore::GraphicContextPtr &gc);
	void get_shader_glsl(std::string &out_vertex_shader, std::string &out_fragment_shader0, std::string &out_fragment_shader1);
	void get_shader_hlsl(std::string &out_vertex_shader, std::string &out_fragment_shader0, std::string &out_fragment_shader1);
	uicore::Size find_texture_size(const uicore::Texture2DPtr &normal_z);
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &vertex_source, const std::string &fragment_source);
	static int find_iterations(int size);

	int tile_size;
	int iterations;
	uicore::ProgramObjectPtr program0, program1;
	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec4f> vertices;
	uicore::FrameBufferPtr fb0, fb1;
	uicore::Texture2DPtr texture0, texture1;
	uicore::BlendStatePtr blend_state;
};
