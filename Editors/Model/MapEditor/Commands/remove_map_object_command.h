
#pragma once

#include "Model/UndoSystem/model_command.h"

class RemoveMapObjectCommand : public ModelCommand
{
public:
	RemoveMapObjectCommand(size_t index);

	void execute() override;
	void rollback() override;

private:
	size_t index = 0;
	MapDescObject removed_object;
};
