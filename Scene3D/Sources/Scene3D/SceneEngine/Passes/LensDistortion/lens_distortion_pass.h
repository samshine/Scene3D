
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class LensDistortionPass : public ScenePass
{
public:
	LensDistortionPass(SceneRender &inout);
	std::string name() const override { return "lensdistort"; }
	void run() override;

private:
	struct UniformBuffer
	{
		float aspect;        // image width/height
		float scale;         // 1/max(f)
		float padding1, padding2;
		uicore::Vec4f k;     // lens distortion coefficient 
		uicore::Vec4f kcube; // cubic distortion value
	};

	SceneRender &inout;

	uicore::ProgramObjectPtr shader;
	uicore::UniformVector<UniformBuffer> uniforms;
};
