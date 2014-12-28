
#include "precomp.h"
#include "add_attachment_command.h"
#include "../app_model.h"

using namespace clan;

AddAttachmentCommand::AddAttachmentCommand(FBXAttachmentPoint attachment) : new_attachment(attachment)
{
}

void AddAttachmentCommand::execute()
{
	AppModel::instance()->desc.attachment_points.push_back(new_attachment);
	AppModel::instance()->update_scene_model();
}

void AddAttachmentCommand::rollback()
{
	AppModel::instance()->desc.attachment_points.pop_back();
	AppModel::instance()->update_scene_model();
}
