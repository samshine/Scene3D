
#pragma once

#include "Scene3D/scene.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class Scene_Impl;

class DiffuseGIPassCS
{
public:
	DiffuseGIPassCS(uicore::GraphicContext &gc, const std::string &shader_path, ResourceContainer &inout);
	~DiffuseGIPassCS();

	void run(uicore::GraphicContext &gc, Scene_Impl *scene);

private:
	void update_buffers(uicore::GraphicContext &gc);
	uicore::ProgramObject compile_and_link(uicore::GraphicContext &gc, const std::string &compute_filename);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<uicore::Texture2D> diffuse_color_gbuffer;
	Resource<uicore::Texture2D> normal_z_gbuffer;
	Resource<uicore::Texture2DArray> shadow_maps;

	// Out:
	Resource<uicore::Texture2D> final_color;

	uicore::ProgramObject init_lpv_program;
	uicore::ProgramObject init_gv_program;
	uicore::ProgramObject propagate_lpv_program;
	uicore::ProgramObject accumulate_lpv_program;
	uicore::ProgramObject render_result_program;
};
