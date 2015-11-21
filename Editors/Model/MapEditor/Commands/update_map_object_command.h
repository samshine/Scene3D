
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateMapObjectCommand : public ModelCommand
{
public:
	UpdateMapObjectCommand(size_t index, MapDescObject object);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	MapDescObject old_object;
	MapDescObject new_object;
};
