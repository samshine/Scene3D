
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateAttachmentCommand : public ModelCommand
{
public:
	UpdateAttachmentCommand(size_t index, uicore::ModelDescAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	uicore::ModelDescAttachmentPoint old_attachment;
	uicore::ModelDescAttachmentPoint new_attachment;
};
