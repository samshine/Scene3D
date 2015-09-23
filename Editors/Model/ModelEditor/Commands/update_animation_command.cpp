
#include "precomp.h"
#include "update_animation_command.h"
#include "../model_app_model.h"

using namespace uicore;

UpdateAnimationCommand::UpdateAnimationCommand(size_t index, ModelDescAnimation animation) : index(index), new_animation(animation)
{
}

void UpdateAnimationCommand::execute()
{
	old_animation = ModelAppModel::instance()->desc.animations.at(index);
	ModelAppModel::instance()->desc.animations.at(index) = new_animation;
	ModelAppModel::instance()->update_scene_model();
}

void UpdateAnimationCommand::rollback()
{
	ModelAppModel::instance()->desc.animations.at(index) = old_animation;
	ModelAppModel::instance()->update_scene_model();
}
