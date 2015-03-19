
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddAnimationCommand : public ModelCommand
{
public:
	AddAnimationCommand(clan::ModelDescAnimation animation);

	void execute() override;
	void rollback() override;

private:
	clan::ModelDescAnimation new_animation;
};
