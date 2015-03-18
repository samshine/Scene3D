
#include "precomp.h"
#include "set_map_model_command.h"
#include "../model_app_model.h"

SetMapModelCommand::SetMapModelCommand(std::string map_model) : new_map_model(std::move(map_model))
{
}

void SetMapModelCommand::execute()
{
	old_map_model = AppModel::instance()->map_model;
	AppModel::instance()->map_model = new_map_model;
	AppModel::instance()->sig_map_model_updated();
}

void SetMapModelCommand::rollback()
{
	AppModel::instance()->map_model = old_map_model;
	AppModel::instance()->sig_map_model_updated();
}
