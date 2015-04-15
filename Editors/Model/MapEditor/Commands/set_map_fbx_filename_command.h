
#pragma once

#include "Model/UndoSystem/model_command.h"

class SetMapFbxFilenameCommand : public ModelCommand
{
public:
	SetMapFbxFilenameCommand(const std::string &filename);

	void execute() override;
	void rollback() override;

private:
	std::string old_filename;
	std::string new_filename;
};
