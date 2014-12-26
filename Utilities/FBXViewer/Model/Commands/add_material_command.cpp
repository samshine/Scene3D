
#include "precomp.h"
#include "add_material_command.h"
#include "../app_model.h"

using namespace clan;

AddMaterialCommand::AddMaterialCommand(FBXMaterial material) : new_material(material)
{
}

void AddMaterialCommand::execute()
{
	AppModel::instance()->desc.materials.push_back(new_material);
	AppModel::instance()->update_scene_model();
}

void AddMaterialCommand::rollback()
{
	AppModel::instance()->desc.materials.pop_back();
	AppModel::instance()->update_scene_model();
}
