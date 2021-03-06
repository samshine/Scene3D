
#include "precomp.h"
#include "texture_builder.h"

using namespace uicore;

void TextureBuilder::build(std::shared_ptr<ModelData> model_data, const std::string &output_path)
{
	for (auto &texture : model_data->textures)
	{
		try
		{
			PixelBufferSetPtr pixelbuffer_set;

			std::string filetype = FilePath::extension(texture.name);
			if (Text::equal_caseless(filetype, "dds"))
			{
				pixelbuffer_set = DDSFormat::load(texture.name);
			}
			else
			{
				pixelbuffer_set = PixelBufferSet::create(ImageFile::load(texture.name, std::string(), texture.gamma != 1.0f));
			}

			auto pixels = pixelbuffer_set->image(0, 0);

			auto file_data = MemoryDevice::create();
			file_data->buffer()->set_capacity(pixels->data_size());
			PNGFormat::save(pixels, file_data);

			auto sha1 = SHA1::create();
			sha1->add(file_data->buffer());
			sha1->calculate();
			std::string hash = sha1->hash();

			std::string filename = hash + ".png";
			File::write_all_bytes(FilePath::combine(output_path, filename), file_data->buffer());
			texture.name = filename;
		}
		catch (const Exception &)
		{
			// To do: log this
		}
	}
}
