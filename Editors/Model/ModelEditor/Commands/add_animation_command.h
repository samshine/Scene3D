
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddAnimationCommand : public ModelCommand
{
public:
	AddAnimationCommand(clan::FBXAnimation animation);

	void execute() override;
	void rollback() override;

private:
	clan::FBXAnimation new_animation;
};
