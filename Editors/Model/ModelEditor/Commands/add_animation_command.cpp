
#include "precomp.h"
#include "add_animation_command.h"
#include "../model_app_model.h"

using namespace clan;

AddAnimationCommand::AddAnimationCommand(ModelDescAnimation animation) : new_animation(animation)
{
}

void AddAnimationCommand::execute()
{
	ModelAppModel::instance()->desc.animations.push_back(new_animation);
	ModelAppModel::instance()->update_scene_model();
}

void AddAnimationCommand::rollback()
{
	ModelAppModel::instance()->desc.animations.pop_back();
	ModelAppModel::instance()->update_scene_model();
}
