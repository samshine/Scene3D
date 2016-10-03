
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class ExposurePass : public ScenePass
{
public:
	ExposurePass(SceneRender &inout);
	std::string name() const override { return "exposure"; }
	void run() override;

private:
	struct UniformBuffer
	{
		float ExposureBase;
		float ExposureMin;
		float ExposureScale;
		float ExposureSpeed;
		uicore::Vec2f Scale;
		uicore::Vec2f Offset;
	};

	void create_extract_shader();
	void create_average_shader();
	void create_combine_shader();

	SceneRender &inout;

	uicore::ProgramObjectPtr extract_shader;
	uicore::ProgramObjectPtr average_shader;
	uicore::ProgramObjectPtr combine_shader;
	uicore::UniformVector<UniformBuffer> uniforms;
};
