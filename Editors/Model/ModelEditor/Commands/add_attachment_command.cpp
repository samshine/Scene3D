
#include "precomp.h"
#include "add_attachment_command.h"
#include "../model_app_model.h"

using namespace clan;

AddAttachmentCommand::AddAttachmentCommand(FBXAttachmentPoint attachment) : new_attachment(attachment)
{
}

void AddAttachmentCommand::execute()
{
	ModelAppModel::instance()->desc.attachment_points.push_back(new_attachment);
	ModelAppModel::instance()->update_scene_model();
}

void AddAttachmentCommand::rollback()
{
	ModelAppModel::instance()->desc.attachment_points.pop_back();
	ModelAppModel::instance()->update_scene_model();
}
