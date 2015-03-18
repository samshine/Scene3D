
#pragma once

class SimpleDisplayCache : public clan::DisplayCache
{
public:
	clan::Resource<clan::Sprite> get_sprite(clan::Canvas &canvas, const std::string &id) override { throw clan::Exception("Sprite resources not supported"); }

	clan::Resource<clan::Image> get_image(clan::Canvas &canvas, const std::string &id) override
	{
		if (images.find(id) == images.end())
		{
			images[id] = clan::Image(canvas, clan::PathHelp::combine("Resources", id));
		}
		return images[id];
	}

	clan::Resource<clan::Texture> get_texture(clan::GraphicContext &gc, const std::string &id) override { throw clan::Exception("Texture resources not supported"); }

	clan::Resource<clan::Font> get_font(clan::Canvas &canvas, const std::string &family_name, const clan::FontDescription &desc) override
	{
		clan::FontFamily &font_family = font_families[family_name];
		if (font_family.is_null())
		{
			font_family = clan::FontFamily(family_name);
			if (font_family.get_family_name() == "Lato")
			{
				clan::FontDescription regular;
				font_family.add(regular, "Resources/Fonts/Lato/Lato-Regular.ttf");

				clan::FontDescription bold;
				bold.set_weight(clan::FontWeight::bold);
				font_family.add(bold, "Resources/Fonts/Lato/Lato-Bold.ttf");

				clan::FontDescription italic;
				italic.set_style(clan::FontStyle::italic);
				font_family.add(italic, "Resources/Fonts/Lato/Lato-Italic.ttf");

				clan::FontDescription light;
				light.set_weight(clan::FontWeight::light);
				font_family.add(light, "Resources/Fonts/Lato/Lato-Light.ttf");

				clan::FontDescription black;
				black.set_weight(clan::FontWeight::heavy);
				font_family.add(black, "Resources/Fonts/Lato/Lato-Black.ttf");
			}
			else
			{
				font_family.add(family_name, desc.get_height());
			}
		}

		return clan::Font(font_family, desc);
	}

private:
	std::map<std::string, clan::Resource<clan::Image>> images;
	std::map<std::string, clan::FontFamily> font_families;
};
