
#include "viewer_scene_cache.h"

using namespace clan;

ViewerSceneCache::ViewerSceneCache()
{
}

std::shared_ptr<ModelData> ViewerSceneCache::get_model_data(const std::string &name)
{
	throw Exception("ViewerSceneCache::get_model_data is not implemented");
}

Resource<Texture> ViewerSceneCache::get_texture(GraphicContext &gc, const std::string &name, bool linear)
{
	auto it = loaded_textures.find(name);
	if (it != loaded_textures.end())
		return it->second;

	try
	{
		ImageImportDescription import_desc;
		import_desc.set_flip_vertical(false);
		import_desc.set_srgb(!linear);

		Resource<Texture> texture = Texture2D(gc, name, import_desc);
		loaded_textures[name] = texture;
		return texture;
	}
	catch (Exception &)
	{
		Resource<Texture> texture = get_dummy_texture(gc);
		loaded_textures[name] = texture;
		return texture;
	}
}

void ViewerSceneCache::update_textures(GraphicContext &gc, float time_elapsed)
{
}

Texture2D ViewerSceneCache::get_dummy_texture(clan::GraphicContext &gc)
{
	if (dummy_texture.is_null())
	{
		PixelBuffer pixelbuffer(64, 64, tf_rgba8);

		PixelBufferLock4ub lock(pixelbuffer);

		for (int y = 0; y < 64; y++)
		{
			Vec4ub *row = lock.get_row(y);
			for (int x = 0; x < 64; x++)
			{
				if (x < 4 || x > 59 || y < 4 || y > 59)
					row[x] = Vec4ub(255, 80, 80, 255);
				else
					row[x] = Vec4ub(80, 80, 80, 255);
			}
		}

		lock.unlock();

		dummy_texture = Texture2D(gc, 64, 64, tf_rgba8, 0);
		dummy_texture.set_image(gc, pixelbuffer);
		dummy_texture.generate_mipmap();
	}

	return dummy_texture;
}
