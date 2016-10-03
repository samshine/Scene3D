
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class LinearDepthPass : public ScenePass
{
public:
	LinearDepthPass(SceneRender &inout);
	std::string name() const override { return "ssao"; }
	void run() override;

private:
	struct UniformBuffer
	{
		float LinearizeDepthA;
		float LinearizeDepthB;
		float InverseDepthRangeA;
		float InverseDepthRangeB;
		uicore::Vec2f Scale;
		uicore::Vec2f Offset;
		int SampleCount;
		int Padding1, Padding2, Padding3;
	};

	SceneRender &inout;

	uicore::ProgramObjectPtr shader;
	uicore::UniformVector<UniformBuffer> uniforms;
};
