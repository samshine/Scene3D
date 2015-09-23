
#pragma once

class SimpleDisplayCache : public uicore::DisplayCache
{
public:
	uicore::Resource<uicore::Sprite> get_sprite(uicore::Canvas &canvas, const std::string &id) override { throw uicore::Exception("Sprite resources not supported"); }

	uicore::Resource<uicore::Image> get_image(uicore::Canvas &canvas, const std::string &id) override
	{
		if (images.find(id) == images.end())
		{
			images[id] = uicore::Image(canvas, uicore::PathHelp::combine("Resources", id));
		}
		return images[id];
	}

	uicore::Resource<uicore::Texture> get_texture(uicore::GraphicContext &gc, const std::string &id) override { throw uicore::Exception("Texture resources not supported"); }

	uicore::Resource<uicore::Font> get_font(uicore::Canvas &canvas, const std::string &family_name, const uicore::FontDescription &desc) override
	{
		uicore::FontFamily &font_family = font_families[family_name];
		if (font_family.is_null())
		{
			font_family = uicore::FontFamily(family_name);
			if (font_family.get_family_name() == "Lato")
			{
				uicore::FontDescription regular;
				font_family.add(regular, "Resources/Fonts/Lato/Lato-Regular.ttf");

				uicore::FontDescription bold;
				bold.set_weight(uicore::FontWeight::bold);
				font_family.add(bold, "Resources/Fonts/Lato/Lato-Bold.ttf");

				uicore::FontDescription italic;
				italic.set_style(uicore::FontStyle::italic);
				font_family.add(italic, "Resources/Fonts/Lato/Lato-Italic.ttf");

				uicore::FontDescription light;
				light.set_weight(uicore::FontWeight::light);
				font_family.add(light, "Resources/Fonts/Lato/Lato-Light.ttf");

				uicore::FontDescription black;
				black.set_weight(uicore::FontWeight::heavy);
				font_family.add(black, "Resources/Fonts/Lato/Lato-Black.ttf");
			}
			else
			{
				font_family.add(family_name, desc.get_height());
			}
		}

		return uicore::Font(font_family, desc);
	}

private:
	std::map<std::string, uicore::Resource<uicore::Image>> images;
	std::map<std::string, uicore::FontFamily> font_families;
};
