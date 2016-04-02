
#include "precomp.h"
#include "animations_controller.h"
#include "Views/ModelEditor/animations_sidepanel.h"
#include "Model/ModelEditor/model_app_model.h"
#include "Controllers/PopupMenu/popup_menu_controller.h"

using namespace uicore;

AnimationsController::AnimationsController()
{
	panel = view->content_view()->add_child<AnimationsSidePanelView>();

	slots.connect(panel->animations_list->sig_selection_changed(), this, &AnimationsController::animations_list_selection_changed);
	slots.connect(panel->animations_list->sig_edit_saved(), this, &AnimationsController::animations_list_edit_saved);
	slots.connect(panel->start_property->sig_value_changed(), this, &AnimationsController::start_property_value_changed);
	slots.connect(panel->end_property->sig_value_changed(), this, &AnimationsController::end_property_value_changed);
	slots.connect(panel->play_property->sig_value_changed(), this, &AnimationsController::play_property_value_changed);
	slots.connect(panel->move_property->sig_value_changed(), this, &AnimationsController::move_property_value_changed);
	slots.connect(panel->loop_property->sig_value_changed(), this, &AnimationsController::loop_property_value_changed);
	slots.connect(panel->rarity_property->sig_value_changed(), this, &AnimationsController::rarity_property_value_changed);
	slots.connect(panel->fbx_filename_property->sig_browse(), this, &AnimationsController::fbx_filename_property_browse);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_animations(); });

	slots.connect(panel->animations_list->sig_pointer_release(), [this](PointerEvent *e)
	{
		if (e->button() == PointerButton::right)
		{
			e->stop_propagation();

			std::vector<PopupMenuItem> menu_items =
			{
				{ "Remove", []() {} },
			};

			WindowManager::present_popup<PopupMenuController>(panel->animations_list.get(), e->pos(panel->animations_list.get()), menu_items);
		}
	});

	update_animations();
}

void AnimationsController::update_animations()
{
	panel->animations_list->clear();
	for (const auto &anim : ModelAppModel::instance()->desc.animations)
	{
		panel->animations_list->add_item(anim.name);
	}
	panel->animations_list->add_item("");
	panel->animations_list->set_selected(0);
	update_animation_fields();
}

void AnimationsController::update_animation_fields()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1 && selection < (int)ModelAppModel::instance()->desc.animations.size())
	{
		panel->animation->set_hidden(false);

		const auto &anim = ModelAppModel::instance()->desc.animations[selection];
		panel->start_property->text_field->set_text(Text::to_string(anim.start_frame));
		panel->end_property->text_field->set_text(Text::to_string(anim.end_frame));
		panel->play_property->text_field->set_text(Text::to_string(anim.play_speed));
		panel->move_property->text_field->set_text(Text::to_string(anim.move_speed));
		panel->loop_property->text_field->set_text(anim.loop ? "1" : "0");
		panel->rarity_property->text_field->set_text(Text::to_string(anim.rarity));
		panel->fbx_filename_property->browse_field->set_text(PathHelp::basename(anim.fbx_filename));
	}
	else
	{
		panel->animation->set_hidden(true);
	}
}

void AnimationsController::animations_list_selection_changed()
{
	update_animation_fields();
}

void AnimationsController::animations_list_edit_saved()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		if (selection >= (int)ModelAppModel::instance()->desc.animations.size())
		{
			ModelDescAnimation anim;
			anim.name = panel->animations_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<AddAnimationCommand>(anim);

			panel->animations_list->add_item("");
		}
		else
		{
			ModelDescAnimation anim = ModelAppModel::instance()->desc.animations[selection];
			anim.name = panel->animations_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<UpdateAnimationCommand>(selection, anim);
		}

		update_animation_fields();
	}
}

void AnimationsController::start_property_value_changed()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.start_frame = panel->start_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::end_property_value_changed()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.end_frame = panel->end_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::play_property_value_changed()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.play_speed = panel->play_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::move_property_value_changed()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.move_speed = panel->move_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::loop_property_value_changed()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.loop = panel->loop_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::rarity_property_value_changed()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.rarity = panel->rarity_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::fbx_filename_property_browse()
{
	auto selection = panel->animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);

		OpenFileDialog dialog(view.get());
		dialog.set_title("Select Model");
		dialog.add_filter("FBX model files (*.fbx)", "*.fbx", true);
		dialog.add_filter("All files (*.*)", "*.*", false);
		dialog.set_filename(animation.fbx_filename);
		if (dialog.show())
		{
			animation.fbx_filename = dialog.filename();
			app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
			panel->fbx_filename_property->browse_field->set_text(PathHelp::basename(animation.fbx_filename));
		}
	}
}
