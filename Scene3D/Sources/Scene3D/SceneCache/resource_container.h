
#pragma once

#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include "Scene3D/Performance/gpu_timer.h"
#include "Scene3D/SceneCache/instances_buffer.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/Model/model_shader_cache.h"
#include "Scene3D/Passes/VSMShadowMap/vsm_shadow_map_pass.h"
#include "Scene3D/Passes/Lightsource/lightsource_pass.h"
#include "Scene3D/Passes/LightsourceSimple/lightsource_simple_pass.h"
#include "Scene3D/Passes/GBuffer/gbuffer_pass.h"
#include "Scene3D/Passes/Skybox/skybox_pass.h"
#include "Scene3D/Passes/Bloom/bloom_pass.h"
#include "Scene3D/Passes/SSAO/ssao_pass.h"
#include "Scene3D/Passes/Final/final_pass.h"
#include "Scene3D/Passes/Transparency/transparency_pass.h"
#include "Scene3D/Passes/ParticleEmitter/particle_emitter_pass.h"
#include "Scene3D/Passes/LensFlare/lens_flare_pass.h"
#include <memory>
#include <map>

class SceneCacheImpl;

class ResourceContainer
{
public:
	ResourceContainer(const uicore::GraphicContextPtr &gc, const std::string &shader_path, SceneCacheImpl *engine);
	ResourceContainer(const ResourceContainer &) = delete;
	ResourceContainer &operator=(const ResourceContainer &) = delete;

	void setup_pass_buffers(const uicore::GraphicContextPtr &gc);

	std::string shader_path;

	uicore::Rect viewport = uicore::Size(640, 480);
	uicore::FrameBufferPtr fb_viewport;

	float field_of_view = 60.0f;
	uicore::Mat4f world_to_eye;

	uicore::FrameBufferPtr fb_gbuffer;
	uicore::FrameBufferPtr fb_self_illumination;
	uicore::FrameBufferPtr fb_bloom_extract;
	uicore::FrameBufferPtr fb_ambient_occlusion;
	uicore::FrameBufferPtr fb_final_color;

	uicore::Texture2DPtr zbuffer;

	uicore::Texture2DPtr diffuse_color_gbuffer;
	uicore::Texture2DPtr specular_color_gbuffer;
	uicore::Texture2DPtr specular_level_gbuffer;
	uicore::Texture2DPtr self_illumination_gbuffer;
	uicore::Texture2DPtr normal_z_gbuffer;

	uicore::Texture2DPtr bloom_contribution;
	uicore::Texture2DPtr ambient_occlusion;
	uicore::Texture2DPtr final_color;

	uicore::Texture2DArrayPtr shadow_maps;

	GaussianBlur blur;

	uicore::Texture2DPtr skybox_texture;
	bool show_skybox_stars = false;

	std::vector<std::shared_ptr<ScenePass>> passes;

	std::unique_ptr<ModelShaderCache> model_shader_cache;
	InstancesBuffer instances_buffer;

	int models_drawn = 0;
	int instances_drawn = 0;
	int draw_calls = 0;
	int triangles_drawn = 0;
	int scene_visits = 0;
	std::vector<GPUTimer::Result> gpu_results;
	GPUTimer gpu_timer;
};
