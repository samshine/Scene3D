
#include "precomp.h"
#include "Controllers/MainWindow/main_window.h"
#include "Model/app_model.h"

using namespace clan;

class SimpleDisplayCache : public DisplayCache
{
public:
	Resource<Sprite> get_sprite(Canvas &canvas, const std::string &id) override { throw Exception("Sprite resources not supported"); }

	Resource<Image> get_image(Canvas &canvas, const std::string &id) override
	{
		if (images.find(id) == images.end())
		{
			images[id] = Image(canvas, PathHelp::combine("Resources", id));
		}
		return images[id];
	}

	Resource<Texture> get_texture(GraphicContext &gc, const std::string &id) override { throw Exception("Texture resources not supported"); }

	Resource<Font> get_font(Canvas &canvas, const std::string &family_name, const FontDescription &desc) override
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
			else
			{
				font_family.add(family_name, desc.get_height());
			}
		}

		return Font(font_family, desc);
	}

private:
	std::map<std::string, Resource<Image>> images;
	std::map<std::string, FontFamily> font_families;
};

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	SetupCore core;
	SetupDisplay display;
	SetupD3D d3d;

	ResourceManager resources;
	DisplayCache::set(resources, std::make_shared<SimpleDisplayCache>());
	UIThread ui_thread(resources);

	AppModel model;

	auto window = std::make_shared<MainWindow>();
	window->window_view()->show();

	RunLoop::run();

	model.editor_scene.reset();

	return 0;
}

