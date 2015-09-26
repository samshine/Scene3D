
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateAttachmentCommand : public ModelCommand
{
public:
	UpdateAttachmentCommand(size_t index, ModelDescAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	ModelDescAttachmentPoint old_attachment;
	ModelDescAttachmentPoint new_attachment;
};
