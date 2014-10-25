
#include "precomp.h"
#include "update_animation_command.h"
#include "../app_model.h"

UpdateAnimationCommand::UpdateAnimationCommand(size_t index, Animation animation) : index(index), new_animation(animation)
{
}

void UpdateAnimationCommand::execute()
{
	old_animation = AppModel::instance()->animations.at(index);
	AppModel::instance()->animations.at(index) = new_animation;
	AppModel::instance()->update_scene_model();
}

void UpdateAnimationCommand::rollback()
{
	AppModel::instance()->animations.at(index) = old_animation;
	AppModel::instance()->update_scene_model();
}
