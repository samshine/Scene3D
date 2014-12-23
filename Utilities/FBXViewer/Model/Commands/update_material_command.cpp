
#include "precomp.h"
#include "update_material_command.h"
#include "../app_model.h"

using namespace clan;

UpdateMaterialCommand::UpdateMaterialCommand(size_t index, FBXMaterial material) : index(index), new_material(material)
{
}

void UpdateMaterialCommand::execute()
{
	old_material = AppModel::instance()->desc.materials.at(index);
	AppModel::instance()->desc.materials.at(index) = new_material;
	AppModel::instance()->update_scene_model();
}

void UpdateMaterialCommand::rollback()
{
	AppModel::instance()->desc.materials.at(index) = old_material;
	AppModel::instance()->update_scene_model();
}
