
#include "precomp.h"
#include "viewer_scene_cache.h"

using namespace uicore;

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
		PixelBuffer image = ImageFile::load(name, std::string(), !linear);
		image.premultiply_alpha();

		// Convert sRGB 16 bit image to linear:
		if (image.get_format() == tf_rgba16 && !linear)
			image.premultiply_gamma(2.2f);

		bool has_mipmaps = is_power_of_two(image.get_width(), image.get_height());

		Texture2D loaded_texture = Texture2D(gc, image.get_width(), image.get_height(), image.get_format(), has_mipmaps ? 0 : 1);
		loaded_texture.set_image(gc, image);
#ifndef DEBUG_FILTER_NEAREST
		if (has_mipmaps)
		{
			loaded_texture.set_min_filter(filter_linear_mipmap_linear);
			loaded_texture.set_mag_filter(filter_linear);
			loaded_texture.generate_mipmap();
			loaded_texture.set_max_anisotropy(4.0f);
		}
		else
		{
			loaded_texture.set_min_filter(filter_linear);
			loaded_texture.set_mag_filter(filter_linear);
		}
#else
		loaded_texture.set_min_filter(filter_nearest);
		loaded_texture.set_mag_filter(filter_nearest);
#endif

		loaded_texture.set_wrap_mode(wrap_repeat, wrap_repeat);

		Resource<Texture> texture;
		texture.set(loaded_texture);
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

Texture2D ViewerSceneCache::get_dummy_texture(uicore::GraphicContext &gc)
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

bool ViewerSceneCache::is_power_of_two(int width, int height)
{
	int pot_width = 0, pot_height = 0;
	while ((1 << pot_width) < width)
		pot_width++;
	while ((1 << pot_height) < height)
		pot_height++;
	return (width == (1 << pot_width) && height == (1 << pot_height));
}
