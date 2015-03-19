
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddAttachmentCommand : public ModelCommand
{
public:
	AddAttachmentCommand(clan::ModelDescAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	clan::ModelDescAttachmentPoint new_attachment;
};
