
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddBoneCommand : public ModelCommand
{
public:
	AddBoneCommand(ModelDescBone bone);

	void execute() override;
	void rollback() override;

private:
	ModelDescBone new_bone;
};
