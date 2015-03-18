
#include "precomp.h"
#include "update_attachment_command.h"
#include "../model_app_model.h"

using namespace clan;

UpdateAttachmentCommand::UpdateAttachmentCommand(size_t index, FBXAttachmentPoint attachment) : index(index), new_attachment(attachment)
{
}

void UpdateAttachmentCommand::execute()
{
	old_attachment = AppModel::instance()->desc.attachment_points.at(index);
	AppModel::instance()->desc.attachment_points.at(index) = new_attachment;
	AppModel::instance()->update_scene_model();
}

void UpdateAttachmentCommand::rollback()
{
	AppModel::instance()->desc.attachment_points.at(index) = old_attachment;
	AppModel::instance()->update_scene_model();
}
