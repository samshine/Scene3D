
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateAttachmentCommand : public ModelCommand
{
public:
	UpdateAttachmentCommand(size_t index, clan::FBXAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	clan::FBXAttachmentPoint old_attachment;
	clan::FBXAttachmentPoint new_attachment;
};
