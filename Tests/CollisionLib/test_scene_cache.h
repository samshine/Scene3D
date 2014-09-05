
#pragma once

class TestSceneCache : public SceneCache
{
public:
	TestSceneCache();

	std::shared_ptr<ModelData> get_model_data(const std::string &name);

	Resource<Texture> get_texture(GraphicContext &gc, const std::string &name, bool linear);

	void update_textures(GraphicContext &gc, float time_elapsed);

private:
	std::shared_ptr<ModelData> create_plane();
	std::shared_ptr<ModelData> create_box();

	std::map<std::string, Resource<Texture>> loaded_textures;
};
