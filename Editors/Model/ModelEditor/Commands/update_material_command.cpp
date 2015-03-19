
#include "precomp.h"
#include "update_material_command.h"
#include "../model_app_model.h"

using namespace clan;

UpdateMaterialCommand::UpdateMaterialCommand(size_t index, FBXMaterial material) : index(index), new_material(material)
{
}

void UpdateMaterialCommand::execute()
{
	old_material = ModelAppModel::instance()->desc.materials.at(index);
	ModelAppModel::instance()->desc.materials.at(index) = new_material;
	ModelAppModel::instance()->update_scene_model();
}

void UpdateMaterialCommand::rollback()
{
	ModelAppModel::instance()->desc.materials.at(index) = old_material;
	ModelAppModel::instance()->update_scene_model();
}
