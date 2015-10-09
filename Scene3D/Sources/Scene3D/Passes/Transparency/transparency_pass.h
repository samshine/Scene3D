
#pragma once

#include "Scene3D/Model/model_mesh_visitor.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/SceneCache/resource_container.h"

class Scene_Impl;

class TransparencyPass : ModelMeshVisitor
{
public:
	TransparencyPass(ResourceContainer &inout);
	void run(const uicore::GraphicContextPtr &gc, Scene_Impl *scene);

private:
	void setup(const uicore::GraphicContextPtr &gc);
	void render(const uicore::GraphicContextPtr &gc, ModelLOD *model_lod, int num_instances);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;
	Resource<uicore::Texture2DPtr> zbuffer;

	// InOut:
	Resource<uicore::Texture2DPtr> final_color;

	Scene_Impl *scene;
	uicore::FrameBufferPtr fb_transparency;
	uicore::GraphicContextPtr gc;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
};
