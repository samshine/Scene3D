
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/scene_render.h"

class SceneLinesPass : public ScenePass
{
public:
	SceneLinesPass(SceneRender &inout);
	std::string name() const override { return "lines"; }
	void run() override;

private:
	void setup();

	struct UniformBlock
	{
		uicore::Mat4f world_to_projection;
	};

	SceneRender &inout;

	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::ProgramObjectPtr program;

	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> positions;
	uicore::VertexArrayVector<uicore::Vec3f> colors;
	int buffer_size = 0;
	uicore::UniformVector<UniformBlock> gpu_uniforms;
};
