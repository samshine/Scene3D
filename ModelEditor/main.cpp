
#include "precomp.h"
#include "../Editors/Controllers/ModelEditor/ModelEditorWindow/model_editor_window.h"
#include "../Editors/Model/ModelEditor/model_app_model.h"
#include "../Editors/editor_resources.h"

using namespace clan;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DTarget::set_current();
	ResourceManager resources;
	DisplayCache::set(resources, std::make_shared<SimpleDisplayCache>());
	UIThread ui_thread(resources);

	AppModel model;

	auto window = std::make_shared<ModelEditorWindow>();
	window->window_view()->show();

	RunLoop::run();

	model.editor_scene.reset();

	return 0;
}

