
#pragma once

#include "model_command.h"

class AddAttachmentCommand : public ModelCommand
{
public:
	AddAttachmentCommand(clan::FBXAttachmentPoint attachment);

	void execute() override;
	void rollback() override;

private:
	clan::FBXAttachmentPoint new_attachment;
};
