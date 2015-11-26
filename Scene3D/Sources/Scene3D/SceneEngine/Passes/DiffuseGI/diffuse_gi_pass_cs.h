
#pragma once

#include "Scene3D/scene.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/Passes/scene_pass.h"

class DiffuseGIPassCS : public ScenePass
{
public:
	DiffuseGIPassCS(const uicore::GraphicContextPtr &gc, SceneRender &inout);
	~DiffuseGIPassCS();

	std::string name() const override { return "diffuse_gi"; }
	void run() override;

private:
	void update_buffers(const uicore::GraphicContextPtr &gc);
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &program_name, const std::string &source);

	SceneRender &inout;

	uicore::ProgramObjectPtr init_lpv_program;
	uicore::ProgramObjectPtr init_gv_program;
	uicore::ProgramObjectPtr propagate_lpv_program;
	uicore::ProgramObjectPtr accumulate_lpv_program;
	uicore::ProgramObjectPtr render_result_program;
};
