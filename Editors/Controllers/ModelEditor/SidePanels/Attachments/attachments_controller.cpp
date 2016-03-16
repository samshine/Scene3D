
#include "precomp.h"
#include "attachments_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_position_property.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

AttachmentsController::AttachmentsController()
{
	attachments = std::make_shared<RolloutView>("ATTACHMENTS");
	attachment = std::make_shared<RolloutView>("ATTACHMENT");

	view->content_view()->add_child(attachments);
	view->content_view()->add_child(attachment);

	attachments_list = std::make_shared<RolloutList>();
	position_property = std::make_shared<RolloutPositionProperty>("POSITION");
	orientation_property = std::make_shared<RolloutPositionProperty>("ORIENTATION");
	bone_name_property = std::make_shared<RolloutTextFieldProperty>("BONE NAME");
	test_model_property = std::make_shared<RolloutBrowseFieldProperty>("TEST MODEL");
	test_scale_property = std::make_shared<RolloutTextFieldProperty>("TEST SCALE");

	attachments->content->add_child(attachments_list);

	attachment->content->add_child(position_property);
	attachment->content->add_child(orientation_property);
	attachment->content->add_child(bone_name_property);
	attachment->content->add_child(test_model_property);
	attachment->content->add_child(test_scale_property);

	slots.connect(attachments_list->sig_selection_changed(), this, &AttachmentsController::attachments_list_selection_changed);
	slots.connect(attachments_list->sig_edit_saved(), this, &AttachmentsController::attachments_list_edit_saved);
	slots.connect(position_property->sig_value_changed(), this, &AttachmentsController::position_property_value_changed);
	slots.connect(orientation_property->sig_value_changed(), this, &AttachmentsController::orientation_property_value_changed);
	slots.connect(bone_name_property->sig_value_changed(), this, &AttachmentsController::bone_name_property_value_changed);
	slots.connect(test_model_property->sig_browse(), this, &AttachmentsController::test_model_property_browse);
	slots.connect(test_scale_property->sig_value_changed(), this, &AttachmentsController::test_scale_property_value_changed);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_attachments(); });

	update_attachments();
}

void AttachmentsController::update_attachments()
{
	attachments_list->clear();
	for (const auto &attachment : ModelAppModel::instance()->desc.attachment_points)
	{
		attachments_list->add_item(attachment.name);
	}
	auto item = attachments_list->add_item("");
	attachments_list->set_selected(0);
}

void AttachmentsController::update_attachment_fields()
{
	auto selection = attachments_list->selected_item();
	if (selection != -1 && selection < (int)ModelAppModel::instance()->desc.attachment_points.size())
	{
		attachment->set_hidden(false);

		const auto &attachment = ModelAppModel::instance()->desc.attachment_points[selection];
		position_property->input_x->set_text(Text::to_string(attachment.position.x));
		position_property->input_y->set_text(Text::to_string(attachment.position.y));
		position_property->input_z->set_text(Text::to_string(attachment.position.z));

		Vec3f rotation = attachment.orientation.to_matrix().get_euler(order_YXZ);
		rotation *= 180.0f / PI;

		orientation_property->input_x->set_text(Text::to_string(rotation.x));
		orientation_property->input_y->set_text(Text::to_string(rotation.y));
		orientation_property->input_z->set_text(Text::to_string(rotation.z));

		bone_name_property->text_field->set_text(attachment.bone_name);

		test_model_property->browse_field->set_text(PathHelp::basename(attachment.test_model));
		test_scale_property->text_field->set_text(Text::to_string(attachment.test_scale));
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
	auto selection = attachments_list->selected_item();
	if (selection != -1)
	{
		if (selection >= (int)ModelAppModel::instance()->desc.attachment_points.size())
		{
			ModelDescAttachmentPoint attachment;
			attachment.name = attachments_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<AddAttachmentCommand>(attachment);

			attachments_list->add_item("");
		}
		else
		{
			ModelDescAttachmentPoint attachment = ModelAppModel::instance()->desc.attachment_points[selection];
			attachment.name = attachments_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
		}

		update_attachment_fields();
	}
}

void AttachmentsController::position_property_value_changed()
{
	auto selection = attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);
		attachment.position.x = position_property->input_x->text_float();
		attachment.position.y = position_property->input_y->text_float();
		attachment.position.z = position_property->input_z->text_float();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}

void AttachmentsController::orientation_property_value_changed()
{
	auto selection = attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);

		Vec3f rotation;
		rotation.x = orientation_property->input_x->text_float();
		rotation.y = orientation_property->input_y->text_float();
		rotation.z = orientation_property->input_z->text_float();
		attachment.orientation.set(rotation, angle_degrees, order_YXZ);

		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}

void AttachmentsController::bone_name_property_value_changed()
{
	auto selection = attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);
		attachment.bone_name = bone_name_property->text_field->text();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}

void AttachmentsController::test_model_property_browse()
{
	auto selection = attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);

		OpenFileDialog dialog(view.get());
		dialog.set_title("Select Model");
		dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
		dialog.add_filter("All files (*.*)", "*.*", false);
		dialog.set_filename(attachment.test_model);
		if (dialog.show())
		{
			attachment.test_model = dialog.filename();
			app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);

			update_attachment_fields();
		}
	}
}

void AttachmentsController::test_scale_property_value_changed()
{
	auto selection = attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);
		attachment.test_scale = test_scale_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}
