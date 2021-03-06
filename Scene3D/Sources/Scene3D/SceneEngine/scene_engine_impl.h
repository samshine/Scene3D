
#pragma once

#include "Scene3D/scene_engine.h"
#include "resource.h"
#include "work_queue.h"
#include "scene_render.h"
#include <unordered_map>

class Model;
class ModelData;
class SceneViewportImpl;
class SceneImpl;

class SceneEngineImpl : public SceneEngine
{
public:
	SceneEngineImpl();
	~SceneEngineImpl();

	void wait_next_frame_ready(const uicore::GraphicContextPtr &gc) override { render.wait_next_frame_ready(gc); }

	int models_drawn() const override { return render.models_drawn; }
	int instances_drawn() const override { return render.instances_drawn; }
	int draw_calls() const override { return render.draw_calls; }
	int triangles_drawn() const override { return render.triangles_drawn; }
	int scene_visits() const override { return render.scene_visits; }
	int model_instance_maps() const override { return !render.frames.empty() ? render.frames.front()->next_model_staging_buffer : 0; }
	const std::vector<GPUTimer::Result> &gpu_results() const override { return render.gpu_results; }

	void set_custom_pass(std::function<void(uicore::Mat4f eye_to_projection, uicore::Mat4f world_to_eye, uicore::FrameBufferPtr pipeline_fb, uicore::Size viewport_size)> callback) override { render.custom_pass = callback; }

	std::shared_ptr<Model> get_model(const std::string &model_name);
	std::shared_ptr<ModelData> get_model_data(const std::string &name);
	Resource<uicore::TexturePtr> get_texture(const uicore::GraphicContextPtr &gc, const std::string &name, bool linear);
	void update_textures(const uicore::GraphicContextPtr &gc, float time_elapsed);

	void render_viewport(const uicore::GraphicContextPtr &gc, SceneViewportImpl *viewport);
	void update_viewport(const uicore::GraphicContextPtr &gc, SceneViewportImpl *viewport, float time_elapsed);

	SceneRender render;

private:
	static std::string to_key(const std::string &material_name, bool linear);
	uicore::Texture2DPtr get_dummy_texture(const uicore::GraphicContextPtr &gc);

	std::unordered_map<std::string, std::shared_ptr<Model> > models;

	uicore::Texture2DPtr dummy_texture;
	std::unordered_map<std::string, Resource<uicore::TexturePtr>> textures;
	//std::vector<VideoTexture> video_textures;

	long long memory_used = 0;

	uicore::GraphicContextPtr gc;
	WorkQueuePtr work_queue = WorkQueue::create();
	friend class CacheLoadTexture;
};

class CacheLoadTexture : public WorkItem
{
public:
	CacheLoadTexture(SceneEngineImpl *cache, Resource<uicore::TexturePtr> texture, const std::string &material_name, bool linear);

	void process_work();
	void work_completed();
private:
	void load_ctexture(const std::string &material_name);
	void load_clanlib_texture(const std::string &material_name);
	void load_dds_texture(const std::string &material_name);
	void load_blp_texture(const std::string &material_name);
	static bool is_power_of_two(int width, int height);

	SceneEngineImpl *cache;
	Resource<uicore::TexturePtr> texture;
	std::string material_name;
	bool linear;
	std::string video_file;

	uicore::PixelBufferSetPtr pixelbuffer_set;
};
