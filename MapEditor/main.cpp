
#include "precomp.h"
#include "../Editors/Controllers/MapEditor/MapEditorWindow/map_editor_window.h"
#include "../Editors/Model/MapEditor/map_app_model.h"

using namespace uicore;

class ApplicationController : uicore::Application
{
public:
	ApplicationController()
	{
		D3DTarget::set_current();

		ui_thread = UIThread("Resources");

		UIThread::add_font_face("font-family: 'Lato'", "Fonts/Lato/Lato-Regular.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: bold", "Fonts/Lato/Lato-Bold.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: italic", "Fonts/Lato/Lato-Italic.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: 300", "Fonts/Lato/Lato-Light.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: 900", "Fonts/Lato/Lato-Black.ttf");

		WindowManager::set_exit_on_last_close();
		WindowManager::present_main<MapEditorWindow>();
	}

	UIThread ui_thread;
	MapAppModel model;
};

ApplicationInstance<ApplicationController> app;
