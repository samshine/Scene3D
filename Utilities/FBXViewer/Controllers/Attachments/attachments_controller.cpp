
#include "precomp.h"
#include "attachments_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/app_model.h"

using namespace clan;

AttachmentsController::AttachmentsController()
{
	view->box_style.set_layout_block();

	attachments = std::make_shared<RolloutView>("ATTACHMENTS");
	attachment = std::make_shared<RolloutView>("ATTACHMENT");

	view->add_subview(attachments);
	view->add_subview(attachment);

	attachments_list = std::make_shared<RolloutList>();
	position_property = std::make_shared<RolloutTextFieldProperty>("POSITION");
	orientation_property = std::make_shared<RolloutTextFieldProperty>("ORIENTATION");
	bone_name_property = std::make_shared<RolloutTextFieldProperty>("BONE NAME");

	attachments->content->add_subview(attachments_list);

	attachment->content->add_subview(position_property);
	attachment->content->add_subview(orientation_property);
	attachment->content->add_subview(bone_name_property);

	slots.connect(attachments_list->sig_selection_changed(), this, &AttachmentsController::attachments_list_selection_changed);
	slots.connect(attachments_list->sig_edit_saved(), this, &AttachmentsController::attachments_list_edit_saved);
	slots.connect(position_property->sig_value_changed(), this, &AttachmentsController::position_property_value_changed);
	slots.connect(orientation_property->sig_value_changed(), this, &AttachmentsController::orientation_property_value_changed);
	slots.connect(bone_name_property->sig_value_changed(), this, &AttachmentsController::bone_name_property_value_changed);

	slots.connect(AppModel::instance()->sig_load_finished, [this]() { update_attachments(); });

	update_attachments();
}

void AttachmentsController::update_attachments()
{
	attachments_list->clear();
	bool first = true;
	for (const auto &attachment : AppModel::instance()->desc.attachment_points)
	{
		auto item = attachments_list->add_item(attachment.name);
		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	attachments_list->add_item("");
}

void AttachmentsController::update_attachment_fields()
{
	auto selection = attachments_list->selection();
	if (selection && selection->index < AppModel::instance()->desc.attachment_points.size())
	{
		attachment->set_hidden(false);

		const auto &attachment = AppModel::instance()->desc.attachment_points[selection->index];
		position_property->text_field->set_text(StringHelp::float_to_text(attachment.position.x));
		orientation_property->text_field->set_text(StringHelp::float_to_text(attachment.orientation.x));
		bone_name_property->text_field->set_text(attachment.bone_name);
	}
	else
	{
		attachment->set_hidden(true);
	}
}

void AttachmentsController::attachments_list_selection_changed()
{
	update_attachment_fields();
}

void AttachmentsController::attachments_list_edit_saved()
{
	auto selection = attachments_list->selection();
	if (selection)
	{
		if (selection->index >= AppModel::instance()->desc.attachment_points.size())
		{
			AppModel::instance()->desc.attachment_points.resize(selection->index + 1);
			attachments_list->add_item("");
		}

		auto &attachment = AppModel::instance()->desc.attachment_points[selection->index];
		attachment.name = selection->text();

		update_attachment_fields();
	}
}

void AttachmentsController::position_property_value_changed()
{
	auto selection = attachments_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection->index);
		attachment.position.x = position_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection->index, attachment);
	}
}

void AttachmentsController::orientation_property_value_changed()
{
	auto selection = attachments_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection->index);
		attachment.orientation.x = orientation_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection->index, attachment);
	}
}

void AttachmentsController::bone_name_property_value_changed()
{
	auto selection = attachments_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection->index);
		attachment.bone_name = bone_name_property->text_field->text();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection->index, attachment);
	}
}
