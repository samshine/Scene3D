
#pragma once

#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/Model/model_mesh_visitor.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class Scene_Impl;

class GBufferPass : ModelMeshVisitor
{
public:
	GBufferPass(ResourceContainer &inout);
	void run(const uicore::GraphicContextPtr &gc, Scene_Impl *scene);

private:
	void setup_gbuffer(const uicore::GraphicContextPtr &gc);
	void render(const uicore::GraphicContextPtr &gc, ModelLOD *model_lod, int num_instances);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;

	// Out:
	Resource<uicore::Texture2DPtr> diffuse_color_gbuffer;
	Resource<uicore::Texture2DPtr> specular_color_gbuffer;
	Resource<uicore::Texture2DPtr> specular_level_gbuffer;
	Resource<uicore::Texture2DPtr> self_illumination_gbuffer;
	Resource<uicore::Texture2DPtr> normal_z_gbuffer;
	Resource<uicore::Texture2DPtr> zbuffer;

	uicore::FrameBufferPtr fb_gbuffer;
	uicore::GraphicContextPtr gc;
	Scene_Impl *scene;
	uicore::BlendStatePtr blend_state;
	uicore::BlendStatePtr early_z_blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;

	struct RenderEntry
	{
		RenderEntry(ModelLOD *model_lod, int num_instances) : model_lod(model_lod), num_instances(num_instances) { }

		ModelLOD *model_lod;
		int num_instances;
	};
	std::vector<RenderEntry> render_list;
};
