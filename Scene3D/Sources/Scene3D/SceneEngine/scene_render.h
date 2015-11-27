
#pragma once

#include "Scene3D/Performance/gpu_timer.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/shadow_maps.h"
#include "Scene3D/SceneEngine/resource.h"
#include "Scene3D/SceneEngine/Model/model_render.h"
#include "Scene3D/SceneEngine/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/SceneEngine/Passes/VSMShadowMap/vsm_shadow_map_pass.h"
#include "Scene3D/SceneEngine/Passes/Lightsource/lightsource_pass.h"
#include "Scene3D/SceneEngine/Passes/LightsourceSimple/lightsource_simple_pass.h"
#include "Scene3D/SceneEngine/Passes/GBuffer/gbuffer_pass.h"
#include "Scene3D/SceneEngine/Passes/Skybox/skybox_pass.h"
#include "Scene3D/SceneEngine/Passes/Bloom/bloom_pass.h"
#include "Scene3D/SceneEngine/Passes/SSAO/ssao_pass.h"
#include "Scene3D/SceneEngine/Passes/Final/final_pass.h"
#include "Scene3D/SceneEngine/Passes/Transparency/transparency_pass.h"
#include "Scene3D/SceneEngine/Passes/ParticleEmitter/particle_emitter_pass.h"
#include "Scene3D/SceneEngine/Passes/LensFlare/lens_flare_pass.h"
#include <memory>
#include <map>

class SceneCameraImpl;
class SceneEngineImpl;
class SceneViewportImpl;

class SceneRender
{
public:
	SceneRender(SceneEngineImpl *engine);

	void render(const uicore::GraphicContextPtr &gc, SceneViewportImpl *viewport);
	void update(const uicore::GraphicContextPtr &gc, SceneViewportImpl *viewport, float time_elapsed);

	SceneEngineImpl *engine = nullptr;
	SceneImpl *scene = nullptr;
	SceneCameraImpl *camera = nullptr;
	uicore::Rect viewport = uicore::Size(640, 480);
	uicore::FrameBufferPtr fb_viewport;

	float field_of_view = 60.0f;
	uicore::Mat4f world_to_eye;

	uicore::GraphicContextPtr gc;
	float time_elapsed = 0.0f;

	uicore::FrameBufferPtr fb_gbuffer;
	uicore::FrameBufferPtr fb_self_illumination;
	uicore::FrameBufferPtr fb_bloom_blur;
	uicore::FrameBufferPtr fb_bloom_extract;
	uicore::FrameBufferPtr fb_ambient_occlusion;
	uicore::FrameBufferPtr fb_final_color;

	uicore::Texture2DPtr zbuffer;

	uicore::Texture2DPtr diffuse_color_gbuffer;
	uicore::Texture2DPtr specular_color_gbuffer;
	uicore::Texture2DPtr specular_level_gbuffer;
	uicore::Texture2DPtr self_illumination_gbuffer;
	uicore::Texture2DPtr normal_z_gbuffer;

	uicore::Texture2DPtr bloom_blur;
	uicore::Texture2DPtr bloom_contribution;
	uicore::Texture2DPtr ambient_occlusion;
	uicore::Texture2DPtr final_color;

	ShadowMaps shadow_maps;
	GaussianBlur blur;

	uicore::Texture2DPtr skybox_texture;
	bool show_skybox_stars = false;

	std::vector<std::shared_ptr<ScenePass>> passes;

	ModelRender model_render;

	int models_drawn = 0;
	int instances_drawn = 0;
	int draw_calls = 0;
	int triangles_drawn = 0;
	int scene_visits = 0;
	std::vector<GPUTimer::Result> gpu_results;
	GPUTimer gpu_timer;

private:
	SceneRender(const SceneRender &) = delete;
	SceneRender &operator=(const SceneRender &) = delete;

	void setup_passes();
	void setup_pass_buffers();
};
