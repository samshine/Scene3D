
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddAnimationCommand : public ModelCommand
{
public:
	AddAnimationCommand(ModelDescAnimation animation);

	void execute() override;
	void rollback() override;

private:
	ModelDescAnimation new_animation;
};
