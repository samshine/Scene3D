
#include "precomp.h"
#include "update_map_object_command.h"
#include "../map_app_model.h"

using namespace uicore;

UpdateMapObjectCommand::UpdateMapObjectCommand(size_t index, MapDescObject object) : index(index), new_object(object)
{
}

void UpdateMapObjectCommand::execute()
{
	old_object = MapAppModel::instance()->desc.objects.at(index);
	MapAppModel::instance()->desc.objects.at(index) = new_object;
	MapAppModel::instance()->sig_object_updated(index);
	MapAppModel::instance()->sig_map_updated();
}

void UpdateMapObjectCommand::rollback()
{
	MapAppModel::instance()->desc.objects.at(index) = old_object;
	MapAppModel::instance()->sig_object_updated(index);
	MapAppModel::instance()->sig_map_updated();
}
