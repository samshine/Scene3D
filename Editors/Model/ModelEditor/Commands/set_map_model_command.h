
#pragma once

#include "Model/UndoSystem/model_command.h"

class SetMapModelCommand : public ModelCommand
{
public:
	SetMapModelCommand(std::string map_model);

	void execute() override;
	void rollback() override;

private:
	std::string new_map_model;
	std::string old_map_model;
};
