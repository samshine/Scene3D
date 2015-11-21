
#include "precomp.h"
#include "set_map_object_mesh_command.h"
#include "../map_app_model.h"

using namespace uicore;

SetMapObjectMeshCommand::SetMapObjectMeshCommand(size_t index, const std::string &filename) : index(index), new_filename(filename)
{
}

void SetMapObjectMeshCommand::execute()
{
	old_filename = MapAppModel::instance()->desc.objects[index].mesh;
	if (old_filename != new_filename)
	{
		MapAppModel::instance()->desc.objects[index].mesh = new_filename;
		MapAppModel::instance()->sig_map_updated();
	}
}

void SetMapObjectMeshCommand::rollback()
{
	if (old_filename != new_filename)
	{
		MapAppModel::instance()->desc.objects[index].mesh = old_filename;
		MapAppModel::instance()->sig_map_updated();
	}
}
