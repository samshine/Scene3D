
#include "precomp.h"
#include "attachments_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_position_property.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
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
	position_property = std::make_shared<RolloutPositionProperty>("POSITION");
	orientation_property = std::make_shared<RolloutPositionProperty>("ORIENTATION");
	bone_name_property = std::make_shared<RolloutTextFieldProperty>("BONE NAME");
	test_model_property = std::make_shared<RolloutBrowseFieldProperty>("TEST MODEL");
	test_scale_property = std::make_shared<RolloutTextFieldProperty>("TEST SCALE");

	attachments->content->add_subview(attachments_list);

	attachment->content->add_subview(position_property);
	attachment->content->add_subview(orientation_property);
	attachment->content->add_subview(bone_name_property);
	attachment->content->add_subview(test_model_property);
	attachment->content->add_subview(test_scale_property);

	slots.connect(attachments_list->sig_selection_changed(), this, &AttachmentsController::attachments_list_selection_changed);
	slots.connect(attachments_list->sig_edit_saved(), this, &AttachmentsController::attachments_list_edit_saved);
	slots.connect(position_property->sig_value_changed(), this, &AttachmentsController::position_property_value_changed);
	slots.connect(orientation_property->sig_value_changed(), this, &AttachmentsController::orientation_property_value_changed);
	slots.connect(bone_name_property->sig_value_changed(), this, &AttachmentsController::bone_name_property_value_changed);
	slots.connect(test_model_property->sig_browse(), this, &AttachmentsController::test_model_property_browse);
	slots.connect(test_scale_property->sig_value_changed(), this, &AttachmentsController::test_scale_property_value_changed);

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
	auto item = attachments_list->add_item("");
	if (first)
		item->set_selected(true, false);
}

void AttachmentsController::update_attachment_fields()
{
	auto selection = attachments_list->selection();
	if (selection && selection->index < AppModel::instance()->desc.attachment_points.size())
	{
		attachment->set_hidden(false);

		const auto &attachment = AppModel::instance()->desc.attachment_points[selection->index];
		position_property->input_x->set_text(StringHelp::float_to_text(attachment.position.x));
		position_property->input_y->set_text(StringHelp::float_to_text(attachment.position.y));
		position_property->input_z->set_text(StringHelp::float_to_text(attachment.position.z));

		Vec3f rotation = attachment.orientation.to_matrix().get_euler(order_YXZ);
		rotation *= 180.0f / PI;

		orientation_property->input_x->set_text(StringHelp::float_to_text(rotation.x));
		orientation_property->input_y->set_text(StringHelp::float_to_text(rotation.y));
		orientation_property->input_z->set_text(StringHelp::float_to_text(rotation.z));

		bone_name_property->text_field->set_text(attachment.bone_name);

		test_model_property->browse_field->set_text(PathHelp::get_basename(attachment.test_model));
		test_scale_property->text_field->set_text(StringHelp::float_to_text(attachment.test_scale));
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
			FBXAttachmentPoint attachment;
			attachment.name = selection->text();
			AppModel::instance()->undo_system.execute<AddAttachmentCommand>(attachment);

			attachments_list->add_item("");
		}
		else
		{
			FBXAttachmentPoint attachment = AppModel::instance()->desc.attachment_points[selection->index];
			attachment.name = selection->text();
			AppModel::instance()->undo_system.execute<UpdateAttachmentCommand>(selection->index, attachment);
		}

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
		attachment.position.x = position_property->input_x->text_float();
		attachment.position.y = position_property->input_y->text_float();
		attachment.position.z = position_property->input_z->text_float();
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

		Vec3f rotation;
		rotation.x = orientation_property->input_x->text_float();
		rotation.y = orientation_property->input_y->text_float();
		rotation.z = orientation_property->input_z->text_float();
		attachment.orientation.set(rotation, angle_degrees, order_YXZ);

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

void AttachmentsController::test_model_property_browse()
{
	auto selection = attachments_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection->index);

		OpenFileDialog dialog(view.get());
		dialog.set_title("Select Model");
		dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
		dialog.add_filter("All files (*.*)", "*.*", false);
		dialog.set_filename(attachment.test_model);
		if (dialog.show())
		{
			attachment.test_model = dialog.get_filename();
			app_model->undo_system.execute<UpdateAttachmentCommand>(selection->index, attachment);

			update_attachment_fields();
		}
	}
}

void AttachmentsController::test_scale_property_value_changed()
{
	auto selection = attachments_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection->index);
		attachment.test_scale = test_scale_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection->index, attachment);
	}
}
