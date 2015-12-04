
#include "precomp.h"
#include "add_map_object_command.h"
#include "../map_app_model.h"

using namespace uicore;

AddMapObjectCommand::AddMapObjectCommand(MapDescObject object) : new_object(object)
{
}

void AddMapObjectCommand::execute()
{
	MapAppModel::instance()->desc.objects.push_back(new_object);
	MapAppModel::instance()->sig_object_added(MapAppModel::instance()->desc.objects.size() - 1);
	MapAppModel::instance()->sig_map_updated();
}

void AddMapObjectCommand::rollback()
{
	MapAppModel::instance()->desc.objects.pop_back();
	MapAppModel::instance()->sig_object_removed(MapAppModel::instance()->desc.objects.size());
	MapAppModel::instance()->sig_map_updated();
}
