
#pragma once

#include "model_command.h"

class UpdateAnimationCommand : public ModelCommand
{
public:
	UpdateAnimationCommand(size_t index, clan::FBXAnimation animation);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	clan::FBXAnimation old_animation;
	clan::FBXAnimation new_animation;
};
