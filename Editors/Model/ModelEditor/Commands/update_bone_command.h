
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateBoneCommand : public ModelCommand
{
public:
	UpdateBoneCommand(size_t index, ModelDescBone bone);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	ModelDescBone old_bone;
	ModelDescBone new_bone;
};
