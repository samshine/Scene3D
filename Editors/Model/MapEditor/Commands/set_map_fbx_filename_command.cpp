
#include "precomp.h"
#include "set_map_fbx_filename_command.h"
#include "../map_app_model.h"

using namespace clan;

SetMapFbxFilenameCommand::SetMapFbxFilenameCommand(const std::string &filename) : new_filename(filename)
{
}

void SetMapFbxFilenameCommand::execute()
{
	old_filename = MapAppModel::instance()->desc.fbx_filename;
	if (old_filename != new_filename)
	{
		MapAppModel::instance()->desc.fbx_filename = new_filename;
		MapAppModel::instance()->map_fbx = std::make_shared<FBXModel>(MapAppModel::instance()->desc.fbx_filename);
		MapAppModel::instance()->update_map_model_data();
	}
}

void SetMapFbxFilenameCommand::rollback()
{
	if (old_filename != new_filename)
	{
		MapAppModel::instance()->desc.fbx_filename = old_filename;
		MapAppModel::instance()->map_fbx = std::make_shared<FBXModel>(MapAppModel::instance()->desc.fbx_filename);
		MapAppModel::instance()->update_map_model_data();
	}
}
