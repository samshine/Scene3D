
#pragma once

#include "Scene3D/scene_cache.h"
#include "resource.h"
#include "work_queue.h"
#include "resource_container.h"
#include <unordered_map>

class Model;
class ModelData;
class SceneImpl;

class SceneCacheImpl : public SceneCache
{
public:
	SceneCacheImpl(const uicore::GraphicContextPtr &gc, const std::string &shader_path);

	int models_drawn() const override { return inout_data.models_drawn; }
	int instances_drawn() const override { return inout_data.instances_drawn; }
	int draw_calls() const override { return inout_data.draw_calls; }
	int triangles_drawn() const override { return inout_data.triangles_drawn; }
	int scene_visits() const override { return inout_data.scene_visits; }
	const std::vector<GPUTimer::Result> &gpu_results() const override { return inout_data.gpu_results; }

	uicore::GraphicContextPtr get_gc() const { return gc; }

	std::shared_ptr<Model> get_model(const std::string &model_name);
	std::shared_ptr<ModelData> get_model_data(const std::string &name);
	Resource<uicore::TexturePtr> get_texture(const uicore::GraphicContextPtr &gc, const std::string &name, bool linear);
	void update_textures(const uicore::GraphicContextPtr &gc, float time_elapsed);

	void render(const uicore::GraphicContextPtr &gc, SceneImpl *scene);
	void update(const uicore::GraphicContextPtr &gc, SceneImpl *scene, float time_elapsed);

	void process_work_completed() { work_queue.process_work_completed(); }

	ResourceContainer inout_data;

private:
	static std::string to_key(const std::string &material_name, bool linear);
	uicore::Texture2DPtr get_dummy_texture(const uicore::GraphicContextPtr &gc);

	std::unordered_map<std::string, std::shared_ptr<Model> > models;

	uicore::Texture2DPtr dummy_texture;
	std::unordered_map<std::string, Resource<uicore::TexturePtr>> textures;
	//std::vector<VideoTexture> video_textures;

	long long memory_used = 0;

	uicore::GraphicContextPtr gc;
	WorkQueue work_queue;
	friend class CacheLoadTexture;
};

class CacheLoadTexture : public WorkItem
{
public:
	CacheLoadTexture(SceneCacheImpl *cache, Resource<uicore::TexturePtr> texture, const std::string &material_name, bool linear);

	void process_work();
	void work_completed();
private:
	void load_ctexture(const std::string &material_name);
	void load_clanlib_texture(const std::string &material_name);
	void load_dds_texture(const std::string &material_name);
	void load_blp_texture(const std::string &material_name);
	static bool is_power_of_two(int width, int height);

	SceneCacheImpl *cache;
	Resource<uicore::TexturePtr> texture;
	std::string material_name;
	bool linear;
	std::string video_file;

	uicore::PixelBufferSetPtr pixelbuffer_set;
};
