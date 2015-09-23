
#include "precomp.h"
#include "game_display_cache.h"
/*
using namespace uicore;

Resource<Sprite> GameDisplayCache::get_sprite(Canvas &canvas, const std::string &id)
{
	throw Exception("Sprite resources not supported");
}

Resource<Image> GameDisplayCache::get_image(Canvas &canvas, const std::string &id)
{
	if (images.find(id) == images.end())
	{
		images[id] = Image(canvas, PathHelp::combine("Resources", id));
	}
	return images[id];
}

Resource<Texture> GameDisplayCache::get_texture(GraphicContext &gc, const std::string &id)
{
	throw Exception("Texture resources not supported");
}

Resource<Font> GameDisplayCache::get_font(Canvas &canvas, const std::string &family_name, const FontDescription &desc)
{
	FontFamily &font_family = font_families[family_name];
	if (font_family.is_null())
	{
		font_family = FontFamily(family_name);
		if (font_family.get_family_name() == "Lato")
		{
			FontDescription regular;
			font_family.add(regular, "Resources/Fonts/Lato/Lato-Regular.ttf");

			FontDescription bold;
			bold.set_weight(FontWeight::bold);
			font_family.add(bold, "Resources/Fonts/Lato/Lato-Bold.ttf");

			FontDescription italic;
			italic.set_style(FontStyle::italic);
			font_family.add(italic, "Resources/Fonts/Lato/Lato-Italic.ttf");

			FontDescription light;
			light.set_weight(FontWeight::light);
			font_family.add(light, "Resources/Fonts/Lato/Lato-Light.ttf");

			FontDescription black;
			black.set_weight(FontWeight::heavy);
			font_family.add(black, "Resources/Fonts/Lato/Lato-Black.ttf");
		}
		else if (font_family.get_family_name() == "LuckiestGuy")
		{
			FontDescription regular;
			font_family.add(regular, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");
		}
		else
		{
			font_family.add(family_name, desc.get_height());
		}
	}

	return Font(font_family, desc);
}
*/