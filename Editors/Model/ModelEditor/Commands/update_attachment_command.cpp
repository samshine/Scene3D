
#include "precomp.h"
#include "update_attachment_command.h"
#include "../model_app_model.h"

using namespace uicore;

UpdateAttachmentCommand::UpdateAttachmentCommand(size_t index, ModelDescAttachmentPoint attachment) : index(index), new_attachment(attachment)
{
}

void UpdateAttachmentCommand::execute()
{
	old_attachment = ModelAppModel::instance()->desc.attachment_points.at(index);
	ModelAppModel::instance()->desc.attachment_points.at(index) = new_attachment;
	ModelAppModel::instance()->update_scene_model();
}

void UpdateAttachmentCommand::rollback()
{
	ModelAppModel::instance()->desc.attachment_points.at(index) = old_attachment;
	ModelAppModel::instance()->update_scene_model();
}
