
#pragma once

#include "model_command.h"
#include "../animation.h"

class UpdateAnimationCommand : public ModelCommand
{
public:
	UpdateAnimationCommand(size_t index, Animation animation);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	Animation old_animation;
	Animation new_animation;
};
