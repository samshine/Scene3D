
#pragma once

#include "Scene3D/Model/model_mesh_visitor.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/SceneCache/resource_container.h"

class Scene_Impl;

class TransparencyPass : ModelMeshVisitor
{
public:
	TransparencyPass(ResourceContainer &inout);
	void run(uicore::GraphicContext &gc, Scene_Impl *scene);

private:
	void setup(uicore::GraphicContext &gc);
	void render(uicore::GraphicContext &gc, ModelLOD *model_lod, int num_instances);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;
	Resource<uicore::Texture2D> zbuffer;

	// InOut:
	Resource<uicore::Texture2D> final_color;

	Scene_Impl *scene;
	uicore::FrameBuffer fb_transparency;
	uicore::GraphicContext gc;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
