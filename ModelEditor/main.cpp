
#include "precomp.h"
#include "../Editors/Controllers/ModelEditor/ModelEditorWindow/model_editor_window.h"
#include "../Editors/Model/ModelEditor/model_app_model.h"

using namespace uicore;

class ApplicationController : uicore::Application
{
public:
	ApplicationController()
	{
		UIThread::add_font_face("font-family: 'Lato'", "Fonts/Lato/Lato-Regular.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: bold", "Fonts/Lato/Lato-Bold.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: italic", "Fonts/Lato/Lato-Italic.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: 300", "Fonts/Lato/Lato-Light.ttf");
		UIThread::add_font_face("font-family: 'Lato'; font-weight: 900", "Fonts/Lato/Lato-Black.ttf");

		WindowManager::set_exit_on_last_close();
		WindowManager::present_main<ModelEditorWindow>();
	}

	ModelAppModel model;
};

ApplicationInstance<ApplicationController> app;
