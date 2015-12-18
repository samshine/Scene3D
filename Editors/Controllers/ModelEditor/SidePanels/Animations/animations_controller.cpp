
#include "precomp.h"
#include "animations_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

AnimationsController::AnimationsController()
{
	animations = std::make_shared<RolloutView>("ANIMATIONS");
	animation = std::make_shared<RolloutView>("ANIMATION");

	view->content_view()->add_subview(animations);
	view->content_view()->add_subview(animation);

	animations_list = std::make_shared<RolloutList>();
	start_property = std::make_shared<RolloutTextFieldProperty>("START FRAME");
	end_property = std::make_shared<RolloutTextFieldProperty>("END FRAME");
	play_property = std::make_shared<RolloutTextFieldProperty>("PLAY SPEED");
	move_property = std::make_shared<RolloutTextFieldProperty>("MOVE SPEED");
	loop_property = std::make_shared<RolloutTextFieldProperty>("LOOP");
	rarity_property = std::make_shared<RolloutTextFieldProperty>("RARITY");

	animations->content->add_subview(animations_list);

	animation->content->add_subview(start_property);
	animation->content->add_subview(end_property);
	animation->content->add_subview(play_property);
	animation->content->add_subview(move_property);
	animation->content->add_subview(loop_property);
	animation->content->add_subview(rarity_property);

	slots.connect(animations_list->sig_selection_changed(), this, &AnimationsController::animations_list_selection_changed);
	slots.connect(animations_list->sig_edit_saved(), this, &AnimationsController::animations_list_edit_saved);
	slots.connect(start_property->sig_value_changed(), this, &AnimationsController::start_property_value_changed);
	slots.connect(end_property->sig_value_changed(), this, &AnimationsController::end_property_value_changed);
	slots.connect(play_property->sig_value_changed(), this, &AnimationsController::play_property_value_changed);
	slots.connect(move_property->sig_value_changed(), this, &AnimationsController::move_property_value_changed);
	slots.connect(loop_property->sig_value_changed(), this, &AnimationsController::loop_property_value_changed);
	slots.connect(rarity_property->sig_value_changed(), this, &AnimationsController::rarity_property_value_changed);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_animations(); });

	update_animations();
}

void AnimationsController::update_animations()
{
	animations_list->clear();
	for (const auto &anim : ModelAppModel::instance()->desc.animations)
	{
		animations_list->add_item(anim.name);
	}
	animations_list->add_item("");
	animations_list->set_selected(0);
}

void AnimationsController::update_animation_fields()
{
	auto selection = animations_list->selected_item();
	if (selection != -1 && selection < (int)ModelAppModel::instance()->desc.animations.size())
	{
		animation->set_hidden(false);

		const auto &anim = ModelAppModel::instance()->desc.animations[selection];
		start_property->text_field->set_text(Text::to_string(anim.start_frame));
		end_property->text_field->set_text(Text::to_string(anim.end_frame));
		play_property->text_field->set_text(Text::to_string(anim.play_speed));
		move_property->text_field->set_text(Text::to_string(anim.move_speed));
		loop_property->text_field->set_text(anim.loop ? "1" : "0");
		rarity_property->text_field->set_text(Text::to_string(anim.rarity));
	}
	else
	{
		animation->set_hidden(true);
	}
}

void AnimationsController::animations_list_selection_changed()
{
	update_animation_fields();
}

void AnimationsController::animations_list_edit_saved()
{
	auto selection = animations_list->selected_item();
	if (selection != -1)
	{
		if (selection >= (int)ModelAppModel::instance()->desc.animations.size())
		{
			ModelDescAnimation anim;
			anim.name = animations_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<AddAnimationCommand>(anim);

			animations_list->add_item("");
		}
		else
		{
			ModelDescAnimation anim = ModelAppModel::instance()->desc.animations[selection];
			anim.name = animations_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<UpdateAnimationCommand>(selection, anim);
		}

		update_animation_fields();
	}
}

void AnimationsController::start_property_value_changed()
{
	auto selection = animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.start_frame = start_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::end_property_value_changed()
{
	auto selection = animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.end_frame = end_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::play_property_value_changed()
{
	auto selection = animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.play_speed = play_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::move_property_value_changed()
{
	auto selection = animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.move_speed = move_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::loop_property_value_changed()
{
	auto selection = animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.loop = loop_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}

void AnimationsController::rarity_property_value_changed()
{
	auto selection = animations_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto animation = app_model->desc.animations.at(selection);
		animation.rarity = rarity_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection, animation);
	}
}
