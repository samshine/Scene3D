
#include "precomp.h"
#include "add_map_material_command.h"
#include "../map_app_model.h"

using namespace clan;

AddMapMaterialCommand::AddMapMaterialCommand(ModelDescMaterial material) : new_material(material)
{
}

void AddMapMaterialCommand::execute()
{
	MapAppModel::instance()->desc.materials.push_back(new_material);
	MapAppModel::instance()->update_scene_model();
}

void AddMapMaterialCommand::rollback()
{
	MapAppModel::instance()->desc.materials.pop_back();
	MapAppModel::instance()->update_scene_model();
}
