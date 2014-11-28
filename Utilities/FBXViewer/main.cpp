
#include "precomp.h"
#include "Views/MainWindow/main_window.h"
#include "Model/app_model.h"

using namespace clan;

class SimpleDisplayCache : public DisplayCache
{
public:
	Resource<Sprite> get_sprite(Canvas &canvas, const std::string &id) override { throw Exception("Sprite resources not supported"); }
	Resource<Image> get_image(Canvas &canvas, const std::string &id) override { throw Exception("Image resources not supported"); }
	Resource<Texture> get_texture(GraphicContext &gc, const std::string &id) override { throw Exception("Texture resources not supported"); }
	Resource<CollisionOutline> get_collision(const std::string &id) override { throw Exception("Collision resources not supported"); }

	Resource<Font> get_font(Canvas &canvas, const FontDescription &desc) override
	{
		if (lato_regular.get().is_null())
		{
			lato_regular.set(Font(canvas, desc, "Resources/Fonts/Lato/Lato-Regular.ttf"));
			canvas.flush();
			canvas.get_gc().flush();
		}
		return lato_regular;
	}

private:
	Resource<Font> lato_regular;
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
	window->show();

	while (!window->exit)
	{
		KeepAlive::process(250);
	}
	return 0;
}

