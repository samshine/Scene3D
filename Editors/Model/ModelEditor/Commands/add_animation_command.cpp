
#include "precomp.h"
#include "add_animation_command.h"
#include "../app_model.h"

using namespace clan;

AddAnimationCommand::AddAnimationCommand(FBXAnimation animation) : new_animation(animation)
{
}

void AddAnimationCommand::execute()
{
	AppModel::instance()->desc.animations.push_back(new_animation);
	AppModel::instance()->update_scene_model();
}

void AddAnimationCommand::rollback()
{
	AppModel::instance()->desc.animations.pop_back();
	AppModel::instance()->update_scene_model();
}
