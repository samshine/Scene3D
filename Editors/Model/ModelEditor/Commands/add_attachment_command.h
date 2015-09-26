
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddAttachmentCommand : public ModelCommand
{
public:
	AddAttachmentCommand(ModelDescAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	ModelDescAttachmentPoint new_attachment;
};
