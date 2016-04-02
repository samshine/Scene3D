
#include "precomp.h"
#include "add_bone_command.h"
#include "../model_app_model.h"

using namespace uicore;

AddBoneCommand::AddBoneCommand(ModelDescBone bone) : new_bone(bone)
{
}

void AddBoneCommand::execute()
{
	ModelAppModel::instance()->desc.bones.push_back(new_bone);
	ModelAppModel::instance()->update_scene_model();
}

void AddBoneCommand::rollback()
{
	ModelAppModel::instance()->desc.bones.pop_back();
	ModelAppModel::instance()->update_scene_model();
}
