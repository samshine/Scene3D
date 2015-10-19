
#pragma once

#include "Scene3D/scene.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/Passes/scene_pass.h"

class SceneImpl;

class DiffuseGIPassCS : public ScenePass
{
public:
	DiffuseGIPassCS(const uicore::GraphicContextPtr &gc, const std::string &shader_path, SceneRender &inout);
	~DiffuseGIPassCS();

	std::string name() const override { return "diffuse_gi"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void update_buffers(const uicore::GraphicContextPtr &gc);
	uicore::ProgramObjectPtr compile_and_link(const uicore::GraphicContextPtr &gc, const std::string &compute_filename);

	SceneRender &inout;

	uicore::ProgramObjectPtr init_lpv_program;
	uicore::ProgramObjectPtr init_gv_program;
	uicore::ProgramObjectPtr propagate_lpv_program;
	uicore::ProgramObjectPtr accumulate_lpv_program;
	uicore::ProgramObjectPtr render_result_program;
};
