
#pragma once

#include "Model/UndoSystem/model_command.h"

class SetMapObjectMeshCommand : public ModelCommand
{
public:
	SetMapObjectMeshCommand(size_t index, const std::string &filename);

	void execute() override;
	void rollback() override;

private:
	size_t index = 0;
	std::string old_filename;
	std::string new_filename;
};
