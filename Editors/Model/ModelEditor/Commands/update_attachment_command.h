
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateAttachmentCommand : public ModelCommand
{
public:
	UpdateAttachmentCommand(size_t index, clan::ModelDescAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	clan::ModelDescAttachmentPoint old_attachment;
	clan::ModelDescAttachmentPoint new_attachment;
};
