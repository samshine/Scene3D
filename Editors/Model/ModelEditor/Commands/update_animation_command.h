
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateAnimationCommand : public ModelCommand
{
public:
	UpdateAnimationCommand(size_t index, ModelDescAnimation animation);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	ModelDescAnimation old_animation;
	ModelDescAnimation new_animation;
};
