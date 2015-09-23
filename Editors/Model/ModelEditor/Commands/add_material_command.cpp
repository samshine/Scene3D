
#include "precomp.h"
#include "add_material_command.h"
#include "../model_app_model.h"

using namespace uicore;

AddMaterialCommand::AddMaterialCommand(ModelDescMaterial material) : new_material(material)
{
}

void AddMaterialCommand::execute()
{
	ModelAppModel::instance()->desc.materials.push_back(new_material);
	ModelAppModel::instance()->update_scene_model();
}

void AddMaterialCommand::rollback()
{
	ModelAppModel::instance()->desc.materials.pop_back();
	ModelAppModel::instance()->update_scene_model();
}
