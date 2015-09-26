
#pragma once

#include "resource.h"
#include "work_queue.h"
#include <unordered_map>

class ModelData;

class SceneCacheImpl
{
public:
	SceneCacheImpl(uicore::GraphicContext &gc, const std::string &shader_path);

	uicore::GraphicContext get_gc() const { return gc; }
	const std::string &get_shader_path() const { return shader_path; }

	std::shared_ptr<ModelData> get_model_data(const std::string &name);
	Resource<uicore::Texture> get_texture(uicore::GraphicContext &gc, const std::string &name, bool linear);
	void update_textures(uicore::GraphicContext &gc, float time_elapsed);

	void process_work_completed() { work_queue.process_work_completed(); }

private:
	static std::string to_key(const std::string &material_name, bool linear);
	uicore::Texture2D get_dummy_texture(uicore::GraphicContext &gc);

	uicore::Texture2D dummy_texture;
	std::unordered_map<std::string, Resource<uicore::Texture>> textures;
	//std::vector<VideoTexture> video_textures;

	long long memory_used = 0;

	uicore::GraphicContext gc;
	std::string shader_path;
	WorkQueue work_queue;
	friend class CacheLoadTexture;
};

class CacheLoadTexture : public WorkItem
{
public:
	CacheLoadTexture(SceneCacheImpl *cache, Resource<uicore::Texture> texture, const std::string &material_name, bool linear);

private:
	void process_work();
	void work_completed();
	void load_ctexture(const std::string &material_name);
	void load_clanlib_texture(const std::string &material_name);
	void load_dds_texture(const std::string &material_name);
	void load_blp_texture(const std::string &material_name);
	static bool is_power_of_two(int width, int height);

	SceneCacheImpl *cache;
	Resource<uicore::Texture> texture;
	std::string material_name;
	bool linear;
	std::string video_file;

	uicore::PixelBufferSet pixelbuffer_set;
};
