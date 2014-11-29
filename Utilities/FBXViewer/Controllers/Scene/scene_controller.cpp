
#include "precomp.h"
#include "scene_controller.h"
#include "Model/app_model.h"
#include "Views/Scene/scene_view.h"

using namespace clan;

SceneController::SceneController()
{
	view = std::make_shared<SceneView>();

	slots.connect(AppModel::instance()->sig_model_data_updated, [this]()
	{
		scene_view()->set_model_data(AppModel::instance()->model_data);
	});
}

std::shared_ptr<SceneView> SceneController::scene_view()
{
	return std::static_pointer_cast<SceneView>(view);
}
