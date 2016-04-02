
#include "precomp.h"
#include "update_bone_command.h"
#include "../model_app_model.h"

using namespace uicore;

UpdateBoneCommand::UpdateBoneCommand(size_t index, ModelDescBone bone) : index(index), new_bone(bone)
{
}

void UpdateBoneCommand::execute()
{
	old_bone = ModelAppModel::instance()->desc.bones.at(index);
	ModelAppModel::instance()->desc.bones.at(index) = new_bone;
	ModelAppModel::instance()->update_scene_model();
}

void UpdateBoneCommand::rollback()
{
	ModelAppModel::instance()->desc.bones.at(index) = old_bone;
	ModelAppModel::instance()->update_scene_model();
}
