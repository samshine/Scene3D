
#pragma once

class DemoScreenController : public ScreenController
{
public:
	DemoScreenController();
	void update() override;

private:
	void compile_emitter_program();
	void compile_particle_update_program();
	void compile_particle_render_program();

	struct Particle
	{
		uicore::Vec3f pos;
		float size;
		uicore::Vec3f velocity;
		float life;

		int particle_subarray_start;
		int particle_subarray_size;
		int emit_position;
		float emit_cooldown;
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

	uicore::StorageBufferPtr emitters_storage;
	uicore::StorageBufferPtr particles_storage;

	uicore::ProgramObjectPtr emitter_program;
	uicore::ProgramObjectPtr particle_update_program;

	uicore::ProgramObjectPtr particle_render_program;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	uicore::UniformVector<Uniforms> gpu_uniforms;

	const int emitter_count = 64;
	const int particle_count = 1024;
};
