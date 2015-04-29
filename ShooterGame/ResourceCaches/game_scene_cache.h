
#pragma once

#include <unordered_map>

class GameSceneCache : public clan::SceneCache
{
public:
	GameSceneCache(clan::GraphicContext &gc);

	std::shared_ptr<clan::ModelData> get_model_data(const std::string &name) override;
	clan::Resource<clan::Texture> get_texture(clan::GraphicContext &gc, const std::string &name, bool linear) override;
	void update_textures(clan::GraphicContext &gc, float time_elapsed) override;

	void process_work_completed() { work_queue.process_work_completed(); }

private:
	static std::string to_key(const std::string &material_name, bool linear);
	clan::Texture2D get_dummy_texture(clan::GraphicContext &gc);

	clan::Texture2D dummy_texture;
	std::unordered_map<std::string, clan::Resource<clan::Texture>> textures;
	//std::vector<VideoTexture> video_textures;

	long long memory_used;

	clan::GraphicContext gc;
	clan::WorkQueue work_queue;

	friend class CacheLoadTexture;
};

class CacheLoadTexture : public clan::WorkItem
{
public:
	CacheLoadTexture(GameSceneCache *cache, clan::Resource<clan::Texture> texture, const std::string &material_name, bool linear);

private:
	void process_work();
	void work_completed();
	void load_ctexture(const std::string &material_name);
	void load_clanlib_texture(const std::string &material_name);
	void load_dds_texture(const std::string &material_name);
	void load_blp_texture(const std::string &material_name);
	static bool is_power_of_two(int width, int height);

	GameSceneCache *cache;
	clan::Resource<clan::Texture> texture;
	std::string material_name;
	bool linear;
	std::string video_file;

	clan::PixelBufferSet pixelbuffer_set;
};
