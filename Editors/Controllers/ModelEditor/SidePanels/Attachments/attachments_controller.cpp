
#include "precomp.h"
#include "attachments_controller.h"
#include "Views/ModelEditor/attachments_sidepanel.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

AttachmentsController::AttachmentsController()
{
	panel = view->content_view()->add_child<AttachmentsSidePanelView>();

	slots.connect(panel->attachments_list->sig_selection_changed(), this, &AttachmentsController::attachments_list_selection_changed);
	slots.connect(panel->attachments_list->sig_edit_saved(), this, &AttachmentsController::attachments_list_edit_saved);
	slots.connect(panel->position_property->sig_value_changed(), this, &AttachmentsController::position_property_value_changed);
	slots.connect(panel->orientation_property->sig_value_changed(), this, &AttachmentsController::orientation_property_value_changed);
	slots.connect(panel->bone_name_property->sig_value_changed(), this, &AttachmentsController::bone_name_property_value_changed);
	slots.connect(panel->test_model_property->sig_browse(), this, &AttachmentsController::test_model_property_browse);
	slots.connect(panel->test_scale_property->sig_value_changed(), this, &AttachmentsController::test_scale_property_value_changed);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_attachments(); });

	update_attachments();
}

void AttachmentsController::update_attachments()
{
	panel->attachments_list->clear();
	for (const auto &attachment : ModelAppModel::instance()->desc.attachment_points)
	{
		panel->attachments_list->add_item(attachment.name);
	}
	auto item = panel->attachments_list->add_item("");
	panel->attachments_list->set_selected(0);
}

void AttachmentsController::update_attachment_fields()
{
	auto selection = panel->attachments_list->selected_item();
	if (selection != -1 && selection < (int)ModelAppModel::instance()->desc.attachment_points.size())
	{
		panel->attachment->set_hidden(false);

		const auto &attachment = ModelAppModel::instance()->desc.attachment_points[selection];
		panel->position_property->input_x->set_text(Text::to_string(attachment.position.x));
		panel->position_property->input_y->set_text(Text::to_string(attachment.position.y));
		panel->position_property->input_z->set_text(Text::to_string(attachment.position.z));

		Vec3f rotation = degrees(attachment.orientation.to_matrix().get_euler(EulerOrder::yxz));

		panel->orientation_property->input_x->set_text(Text::to_string(rotation.x));
		panel->orientation_property->input_y->set_text(Text::to_string(rotation.y));
		panel->orientation_property->input_z->set_text(Text::to_string(rotation.z));

		panel->bone_name_property->text_field->set_text(attachment.bone_name);

		panel->test_model_property->browse_field->set_text(PathHelp::basename(attachment.test_model));
		panel->test_scale_property->text_field->set_text(Text::to_string(attachment.test_scale));
	}
	else
	{
		panel->attachment->set_hidden(true);
	}
}

void AttachmentsController::attachments_list_selection_changed()
{
	update_attachment_fields();
}

void AttachmentsController::attachments_list_edit_saved()
{
	auto selection = panel->attachments_list->selected_item();
	if (selection != -1)
	{
		if (selection >= (int)ModelAppModel::instance()->desc.attachment_points.size())
		{
			ModelDescAttachmentPoint attachment;
			attachment.name = panel->attachments_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<AddAttachmentCommand>(attachment);

			panel->attachments_list->add_item("");
		}
		else
		{
			ModelDescAttachmentPoint attachment = ModelAppModel::instance()->desc.attachment_points[selection];
			attachment.name = panel->attachments_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
		}

		update_attachment_fields();
	}
}

void AttachmentsController::position_property_value_changed()
{
	auto selection = panel->attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);
		attachment.position.x = panel->position_property->input_x->text_float();
		attachment.position.y = panel->position_property->input_y->text_float();
		attachment.position.z = panel->position_property->input_z->text_float();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}

void AttachmentsController::orientation_property_value_changed()
{
	auto selection = panel->attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);

		Vec3f rotation;
		rotation.x = panel->orientation_property->input_x->text_float();
		rotation.y = panel->orientation_property->input_y->text_float();
		rotation.z = panel->orientation_property->input_z->text_float();
		attachment.orientation = Quaternionf::euler(radians(rotation));

		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}

void AttachmentsController::bone_name_property_value_changed()
{
	auto selection = panel->attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);
		attachment.bone_name = panel->bone_name_property->text_field->text();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}

void AttachmentsController::test_model_property_browse()
{
	auto selection = panel->attachments_list->selected_item();
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
	auto selection = panel->attachments_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto attachment = app_model->desc.attachment_points.at(selection);
		attachment.test_scale = panel->test_scale_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAttachmentCommand>(selection, attachment);
	}
}
