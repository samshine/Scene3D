
#pragma once

#include <unordered_map>
#include "work_queue.h"

class GameSceneCache : public uicore::SceneCache
{
public:
	GameSceneCache(uicore::GraphicContext &gc);

	std::shared_ptr<uicore::ModelData> get_model_data(const std::string &name) override;
	uicore::Resource<uicore::Texture> get_texture(uicore::GraphicContext &gc, const std::string &name, bool linear) override;
	void update_textures(uicore::GraphicContext &gc, float time_elapsed) override;

	void process_work_completed() { work_queue.process_work_completed(); }

private:
	static std::string to_key(const std::string &material_name, bool linear);
	uicore::Texture2D get_dummy_texture(uicore::GraphicContext &gc);

	uicore::Texture2D dummy_texture;
	std::unordered_map<std::string, uicore::Resource<uicore::Texture>> textures;
	//std::vector<VideoTexture> video_textures;

	long long memory_used;

	uicore::GraphicContext gc;
	uicore::WorkQueue work_queue;

	friend class CacheLoadTexture;
};

class CacheLoadTexture : public uicore::WorkItem
{
public:
	CacheLoadTexture(GameSceneCache *cache, uicore::Resource<uicore::Texture> texture, const std::string &material_name, bool linear);

private:
	void process_work();
	void work_completed();
	void load_ctexture(const std::string &material_name);
	void load_clanlib_texture(const std::string &material_name);
	void load_dds_texture(const std::string &material_name);
	void load_blp_texture(const std::string &material_name);
	static bool is_power_of_two(int width, int height);

	GameSceneCache *cache;
	uicore::Resource<uicore::Texture> texture;
	std::string material_name;
	bool linear;
	std::string video_file;

	uicore::PixelBufferSet pixelbuffer_set;
};
