
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddAnimationCommand : public ModelCommand
{
public:
	AddAnimationCommand(uicore::ModelDescAnimation animation);

	void execute() override;
	void rollback() override;

private:
	uicore::ModelDescAnimation new_animation;
};
