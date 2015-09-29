
#include "precomp.h"
#include "texture_builder.h"

using namespace uicore;

void TextureBuilder::build(std::shared_ptr<ModelData> model_data, const std::string &output_path)
{
	for (auto &texture : model_data->textures)
	{
		try
		{
			PixelBufferSet pixelbuffer_set;

			std::string filetype = PathHelp::get_extension(texture.name);
			if (StringHelp::compare(filetype, "dds", true) == 0)
			{
				pixelbuffer_set = DDSFormat::load(texture.name);
			}
			else
			{
				pixelbuffer_set = PixelBufferSet(ImageFile::load(texture.name, std::string(), texture.gamma != 1.0f));
			}

			auto pixels = pixelbuffer_set.get_image(0, 0);

			MemoryDevice file_data;
			file_data.get_data().set_capacity(pixels.get_data_size());
			PNGFormat::save(pixels, file_data);

			SHA1 sha1;
			sha1.add(file_data.get_data());
			sha1.calculate();
			std::string hash = sha1.get_hash();

			std::string filename = hash + ".png";
			File::write_bytes(PathHelp::combine(output_path, filename), file_data.get_data());
			texture.name = filename;
		}
		catch (const Exception &)
		{
			// To do: log this
		}
	}
}
