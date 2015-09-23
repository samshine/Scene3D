
#pragma once

#include <scene3d.h>

class ViewerSceneCache : public uicore::SceneCache
{
public:
	ViewerSceneCache();

	std::shared_ptr<uicore::ModelData> get_model_data(const std::string &name);
	uicore::Resource<uicore::Texture> get_texture(uicore::GraphicContext &gc, const std::string &name, bool linear);
	void update_textures(uicore::GraphicContext &gc, float time_elapsed);

private:
	uicore::Texture2D get_dummy_texture(uicore::GraphicContext &gc);
	static bool is_power_of_two(int width, int height);

	uicore::Texture2D dummy_texture;
	std::map<std::string, uicore::Resource<uicore::Texture> > loaded_textures;
};
