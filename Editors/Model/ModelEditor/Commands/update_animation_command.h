
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateAnimationCommand : public ModelCommand
{
public:
	UpdateAnimationCommand(size_t index, clan::ModelDescAnimation animation);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	clan::ModelDescAnimation old_animation;
	clan::ModelDescAnimation new_animation;
};
