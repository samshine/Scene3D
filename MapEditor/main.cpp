
#include "precomp.h"
#include "../Editors/Controllers/MapEditor/MapEditorWindow/map_editor_window.h"
#include "../Editors/Model/MapEditor/map_app_model.h"
#include "../Editors/editor_resources.h"

using namespace clan;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DTarget::set_current();
	ResourceManager resources;
	DisplayCache::set(resources, std::make_shared<SimpleDisplayCache>());
	UIThread ui_thread(resources);

	MapAppModel model;

	auto window = std::make_shared<MapEditorWindow>();
	window->window_view()->show();

	RunLoop::run();

	return 0;
}

