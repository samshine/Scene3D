
#include "precomp.h"
#include "remove_map_object_command.h"
#include "../map_app_model.h"

using namespace uicore;

RemoveMapObjectCommand::RemoveMapObjectCommand(size_t index) : index(index)
{
}

void RemoveMapObjectCommand::execute()
{
	removed_object = MapAppModel::instance()->desc.objects[index];
	MapAppModel::instance()->desc.objects.erase(MapAppModel::instance()->desc.objects.begin() + index);
	MapAppModel::instance()->sig_object_removed(index);
	MapAppModel::instance()->sig_map_updated();
}

void RemoveMapObjectCommand::rollback()
{
	MapAppModel::instance()->desc.objects.insert(MapAppModel::instance()->desc.objects.begin() + index, removed_object);
	MapAppModel::instance()->sig_object_added(index);
	MapAppModel::instance()->sig_map_updated();
}
