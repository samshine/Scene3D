
#pragma once

class GPUCollision;

class DemoScreenController : public ScreenController
{
public:
	DemoScreenController();
	void update() override;

private:
	struct Particle
	{
		uicore::Vec3f pos;
		float size;
		uicore::Vec3f velocity;
		float life;
	};

	struct Uniforms
	{
		uicore::Mat4f object_to_eye;
		uicore::Mat4f eye_to_projection;
	};

	ScenePtr scene;
	SceneCameraPtr camera;
	SceneObjectPtr box;
	SceneLightPtr light1, light2;

	uicore::FontPtr font;
	uicore::Texture2DPtr particle_texture;

	uicore::ProgramObjectPtr program;
	uicore::StorageBufferPtr storage;
	uicore::StagingBufferPtr staging;

	uicore::ProgramObjectPtr program2;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	uicore::UniformVector<Uniforms> gpu_uniforms;

	const int particle_count = 1024;

	std::shared_ptr<GPUCollision> collision;
};
