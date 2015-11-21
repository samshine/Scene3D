
#include "precomp.h"
#include "add_map_material_command.h"
#include "../map_app_model.h"

using namespace uicore;

AddMapMaterialCommand::AddMapMaterialCommand(ModelDescMaterial material) : new_material(material)
{
}

void AddMapMaterialCommand::execute()
{
	MapAppModel::instance()->desc.materials.push_back(new_material);
	MapAppModel::instance()->sig_map_updated();
}

void AddMapMaterialCommand::rollback()
{
	MapAppModel::instance()->desc.materials.pop_back();
	MapAppModel::instance()->sig_map_updated();
}
