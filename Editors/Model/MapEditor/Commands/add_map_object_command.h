
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMapObjectCommand : public ModelCommand
{
public:
	AddMapObjectCommand(MapDescObject object);

	void execute() override;
	void rollback() override;

private:
	MapDescObject new_object;
};
