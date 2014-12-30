
#include "precomp.h"
#include "environment_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Model/app_model.h"
#include "Model/Commands/set_map_model_command.h"

using namespace clan;

EnvironmentController::EnvironmentController()
{
	view->box_style.set_layout_block();

	environment = std::make_shared<RolloutView>("ENVIRONMENT");
	map_model_property = std::make_shared<RolloutBrowseFieldProperty>("MAP MODEL");

	environment->content->add_subview(map_model_property);
	view->add_subview(environment);

	slots.connect(map_model_property->sig_browse(), this, &EnvironmentController::map_model_property_browse);
	slots.connect(AppModel::instance()->sig_map_model_updated, this, &EnvironmentController::map_model_updated);

	map_model_updated();
}

void EnvironmentController::map_model_property_browse()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Map Model");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(AppModel::instance()->map_model);
	if (dialog.show())
	{
		AppModel::instance()->undo_system.execute<SetMapModelCommand>(dialog.get_filename());
	}
}

void EnvironmentController::map_model_updated()
{
	map_model_property->browse_field->set_text(PathHelp::get_basename(AppModel::instance()->map_model));
}
