
#pragma once

#include "Scene3D/Performance/gpu_timer.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/shadow_maps.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/Model/model_render.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneEngine/Passes/ShadowMap/shadow_map_pass.h"
#include "Scene3D/SceneEngine/Passes/Lightsource/lightsource_pass.h"
#include "Scene3D/SceneEngine/Passes/LightsourceSimple/lightsource_simple_pass.h"
#include "Scene3D/SceneEngine/Passes/GBuffer/gbuffer_pass.h"
#include "Scene3D/SceneEngine/Passes/Decals/decals_pass.h"
#include "Scene3D/SceneEngine/Passes/Skybox/skybox_pass.h"
#include "Scene3D/SceneEngine/Passes/Bloom/bloom_pass.h"
#include "Scene3D/SceneEngine/Passes/SSAO/ssao_pass.h"
#include "Scene3D/SceneEngine/Passes/Final/final_pass.h"
#include "Scene3D/SceneEngine/Passes/Transparency/transparency_pass.h"
#include "Scene3D/SceneEngine/Passes/ParticleEmitter/particle_emitter_pass.h"
#include "Scene3D/SceneEngine/Passes/LensFlare/lens_flare_pass.h"
#include "Scene3D/SceneEngine/Passes/SceneLines/scene_lines_pass.h"
#include "Scene3D/SceneEngine/Passes/LensDistortion/lens_distortion_pass.h"
#include "Scene3D/SceneEngine/Passes/Exposure/exposure_pass.h"
#include <memory>
#include <map>

class SceneCameraImpl;
class SceneEngineImpl;
class SceneViewportImpl;

class SceneRenderFrame
{
public:
	enum { bloom_levels = 4 };
	enum { num_pipeline_buffers = 2};

	uicore::Texture2DArrayPtr shadow_maps;
	uicore::Texture2DPtr shadow_map_blur_texture;
	uicore::FrameBufferPtr fb_shadow_map_blur;

	std::vector<uicore::FrameBufferPtr> fb_shadow_map;
	std::vector<uicore::Texture2DPtr> shadow_map_view;

	uicore::FrameBufferPtr fb_gbuffer;
	uicore::FrameBufferPtr fb_decals;
	uicore::FrameBufferPtr fb_self_illumination;
	uicore::FrameBufferPtr fb_bloom_blurv[bloom_levels];
	uicore::FrameBufferPtr fb_bloom_blurh[bloom_levels];
	uicore::FrameBufferPtr fb_ambient_occlusion;
	uicore::FrameBufferPtr fb_pipeline[num_pipeline_buffers];

	uicore::Texture2DPtr zbuffer;

	uicore::Texture2DPtr diffuse_color_gbuffer;
	uicore::Texture2DPtr specular_color_gbuffer;
	uicore::Texture2DPtr specular_level_gbuffer;
	uicore::Texture2DPtr self_illumination_gbuffer;
	uicore::Texture2DPtr normal_gbuffer;
	uicore::Texture2DPtr face_normal_z_gbuffer;
	uicore::Texture2DPtr ambient_occlusion;

	uicore::Texture2DPtr pipeline[num_pipeline_buffers];
	int current_pipeline_index = 0;

	const uicore::Texture2DPtr &current_pipeline_texture() { return pipeline[current_pipeline_index]; }
	const uicore::FrameBufferPtr &current_pipeline_fb() { return fb_pipeline[current_pipeline_index]; }
	const uicore::FrameBufferPtr &next_pipeline_fb() { return fb_pipeline[(current_pipeline_index + 1) % num_pipeline_buffers]; }
	void next_pipeline_buffer() { current_pipeline_index = (current_pipeline_index + 1) % num_pipeline_buffers; }

	uicore::Texture2DPtr bloom_blurv[bloom_levels];
	uicore::Texture2DPtr bloom_blurh[bloom_levels];

	std::vector<uicore::Texture2DPtr> model_instance_buffers;
	std::vector<uicore::StagingTexturePtr> model_staging_buffers;
	std::vector<uicore::UniformVector<ModelRenderUniforms>> model_render_uniforms;
	int next_model_instance_buffer = 0;
	int next_model_staging_buffer = 0;
	int next_model_render_uniforms = 0;

	std::vector<uicore::Texture2DPtr> decal_instance_buffers;
	std::vector<uicore::StagingTexturePtr> decal_staging_buffers;
	std::vector<uicore::UniformVector<DecalRenderUniforms>> decal_render_uniforms;
	int next_decal_instance_buffer = 0;
	int next_decal_staging_buffer = 0;
	int next_decal_render_uniforms = 0;

	uicore::UniformVector<LightsourceUniforms> compute_uniforms;
	uicore::StorageVector<LightsourceGPULight> compute_lights;
	uicore::StagingVector<LightsourceGPULight> transfer_lights;
	uicore::StorageVector<unsigned int> compute_visible_lights;
	int compute_visible_lights_size = 0;
	std::shared_ptr<ZMinMax> zminmax;

	uicore::Texture2DPtr particle_instance_texture;
	uicore::StagingTexturePtr particle_instance_transfer;

	uicore::ComPtr<ID3D11Query> frame_finished;

	void setup_pass_buffers(SceneRender *render);
};

class SceneRender
{
public:
	SceneRender(SceneEngineImpl *engine);

	void wait_next_frame_ready(const uicore::GraphicContextPtr &gc);

	void render(const uicore::GraphicContextPtr &gc, SceneViewportImpl *viewport);
	void update(const uicore::GraphicContextPtr &gc, SceneViewportImpl *viewport, float time_elapsed);

	SceneEngineImpl *engine = nullptr;
	SceneImpl *scene = nullptr;
	SceneCameraImpl *camera = nullptr;
	SceneViewportImpl *scene_viewport = nullptr;
	uicore::Size viewport_size = uicore::Size(640, 480);
	uicore::Rect final_viewport = uicore::Size(640, 480);
	uicore::FrameBufferPtr fb_viewport;

	float field_of_view = 60.0f;
	uicore::Mat4f world_to_eye;

	uicore::GraphicContextPtr gc;
	float time_elapsed = 0.0f;

	GaussianBlur blur;
	std::shared_ptr<ShadowMaps> shadow_maps;

	uicore::Texture2DPtr skybox_texture;
	bool show_skybox_stars = false;

	uicore::VertexArrayVector<uicore::Vec4f> screen_quad_positions;
	uicore::PrimitivesArrayPtr screen_quad_primitives;
	uicore::BlendStatePtr no_blend;
	uicore::RasterizerStatePtr no_cull_rasterizer;

	std::vector<std::shared_ptr<ScenePass>> passes;
	std::deque<std::shared_ptr<SceneRenderFrame>> frames;

	ModelRender model_render;

	int models_drawn = 0;
	int instances_drawn = 0;
	int draw_calls = 0;
	int triangles_drawn = 0;
	int scene_visits = 0;
	std::vector<GPUTimer::Result> gpu_results;
	GPUTimer gpu_timer;

	std::function<void(uicore::Mat4f eye_to_projection, uicore::Mat4f world_to_eye, uicore::FrameBufferPtr pipeline_fb, uicore::Size viewport_size)> custom_pass;

private:
	SceneRender(const SceneRender &) = delete;
	SceneRender &operator=(const SceneRender &) = delete;

	void setup_screen_quad(const uicore::GraphicContextPtr &gc);
	void setup_passes();
};

class CustomPass : public ScenePass
{
public:
	CustomPass(SceneRender &inout) : inout(inout) { }

	std::string name() const override { return "custom"; }
	void run() override;

	SceneRender &inout;
};