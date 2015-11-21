
#include "precomp.h"
#include "update_map_material_command.h"
#include "../map_app_model.h"

using namespace uicore;

UpdateMapMaterialCommand::UpdateMapMaterialCommand(size_t index, ModelDescMaterial material) : index(index), new_material(material)
{
}

void UpdateMapMaterialCommand::execute()
{
	old_material = MapAppModel::instance()->desc.materials.at(index);
	MapAppModel::instance()->desc.materials.at(index) = new_material;
	MapAppModel::instance()->sig_map_updated();
}

void UpdateMapMaterialCommand::rollback()
{
	MapAppModel::instance()->desc.materials.at(index) = old_material;
	MapAppModel::instance()->sig_map_updated();
}
