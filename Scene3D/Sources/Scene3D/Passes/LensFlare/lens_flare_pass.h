
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/SceneCache/resource_container.h"

class LensFlarePass : public ScenePass
{
public:
	LensFlarePass(const std::string &shader_path, ResourceContainer &inout);
	std::string name() const override { return "lensflare"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;

private:
	void setup(const uicore::GraphicContextPtr &gc);

	struct UniformBlock
	{
		uicore::Mat4f object_to_eye;
		uicore::Mat4f eye_to_projection;
	};

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;

	// InOut:
	Resource<uicore::Texture2DPtr> final_color;

	std::string shader_path;

	uicore::FrameBufferPtr fb;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::ProgramObjectPtr program;

	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	uicore::UniformVector<UniformBlock> gpu_uniforms;

	uicore::Texture2DPtr instance_texture;
	uicore::StagingTexturePtr instance_transfer;

	Resource<uicore::TexturePtr> flare_texture; // To do: should be part of Light
};
