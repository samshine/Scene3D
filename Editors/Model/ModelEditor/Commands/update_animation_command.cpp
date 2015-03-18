
#include "precomp.h"
#include "update_animation_command.h"
#include "../model_app_model.h"

using namespace clan;

UpdateAnimationCommand::UpdateAnimationCommand(size_t index, FBXAnimation animation) : index(index), new_animation(animation)
{
}

void UpdateAnimationCommand::execute()
{
	old_animation = AppModel::instance()->desc.animations.at(index);
	AppModel::instance()->desc.animations.at(index) = new_animation;
	AppModel::instance()->update_scene_model();
}

void UpdateAnimationCommand::rollback()
{
	AppModel::instance()->desc.animations.at(index) = old_animation;
	AppModel::instance()->update_scene_model();
}
