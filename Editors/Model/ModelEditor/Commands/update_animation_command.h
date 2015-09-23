
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateAnimationCommand : public ModelCommand
{
public:
	UpdateAnimationCommand(size_t index, uicore::ModelDescAnimation animation);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	uicore::ModelDescAnimation old_animation;
	uicore::ModelDescAnimation new_animation;
};
