
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/scene_render.h"

class LensFlarePass : public ScenePass
{
public:
	LensFlarePass(SceneRender &inout);
	std::string name() const override { return "lensflare"; }
	void run() override;

private:
	void setup();

	struct UniformBlock
	{
		uicore::Mat4f object_to_eye;
		uicore::Mat4f eye_to_projection;
	};

	SceneRender &inout;

	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::ProgramObjectPtr program;

	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	uicore::UniformVector<UniformBlock> gpu_uniforms;

	uicore::Texture2DPtr instance_texture;
	enum { num_instance_transfer = 4 };
	uicore::StagingTexturePtr instance_transfer[num_instance_transfer];
	int current_instance_transfer = 0;

	Resource<uicore::TexturePtr> flare_texture; // To do: should be part of Light
};
