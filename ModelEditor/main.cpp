
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

	Resource<Font> get_font(Canvas &canvas, const FontDescription &desc) override
	{
		FontFace &font_face = font_faces[desc.get_typeface_name()];
		if (font_face.is_null())
		{
			font_face = FontFace(desc.get_typeface_name());
			if (font_face.get_family_name() == "Lato")
			{
				FontDescription regular;
				regular.set_typeface_name("Lato");
				font_face.add(regular, "Resources/Fonts/Lato/Lato-Regular.ttf");

				FontDescription bold;
				bold.set_typeface_name("Lato");
				bold.set_weight(FontWeight::bold);
				font_face.add(bold, "Resources/Fonts/Lato/Lato-Bold.ttf");

				FontDescription italic;
				italic.set_typeface_name("Lato");
				italic.set_style(FontStyle::italic);
				font_face.add(italic, "Resources/Fonts/Lato/Lato-Italic.ttf");

				FontDescription light;
				light.set_typeface_name("Lato");
				light.set_weight(FontWeight::light);
				font_face.add(light, "Resources/Fonts/Lato/Lato-Light.ttf");

				FontDescription black;
				black.set_typeface_name("Lato");
				black.set_weight(FontWeight::heavy);
				font_face.add(black, "Resources/Fonts/Lato/Lato-Black.ttf");
			}
			else
			{
				font_face.add(desc);
			}
		}

		return Font(font_face, desc);
	}

private:
	std::map<std::string, Resource<Image>> images;
	std::map<std::string, FontFace> font_faces;
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

	while (!window->exit)
	{
		KeepAlive::process(250);
	}

	model.editor_scene.reset();

	return 0;
}

