
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddAttachmentCommand : public ModelCommand
{
public:
	AddAttachmentCommand(uicore::ModelDescAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	uicore::ModelDescAttachmentPoint new_attachment;
};
