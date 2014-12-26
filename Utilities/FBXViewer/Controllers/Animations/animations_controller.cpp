
#include "precomp.h"
#include "animations_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/app_model.h"

using namespace clan;

AnimationsController::AnimationsController()
{
	view->box_style.set_layout_block();

	animations = std::make_shared<RolloutView>("ANIMATIONS");
	animation = std::make_shared<RolloutView>("ANIMATION");

	view->add_subview(animations);
	view->add_subview(animation);

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

	slots.connect(AppModel::instance()->sig_load_finished, [this]() { update_animations(); });

	update_animations();
}

void AnimationsController::update_animations()
{
	animations_list->clear();
	bool first = true;
	for (const auto &anim : AppModel::instance()->desc.animations)
	{
		auto item = animations_list->add_item(anim.name);
		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}

	auto item = animations_list->add_item("");
	if (first)
		item->set_selected(true, false);
}

void AnimationsController::update_animation_fields()
{
	auto selection = animations_list->selection();
	if (selection && selection->index < AppModel::instance()->desc.animations.size())
	{
		animation->set_hidden(false);

		const auto &anim = AppModel::instance()->desc.animations[selection->index];
		start_property->text_field->set_text(StringHelp::int_to_text(anim.start_frame));
		end_property->text_field->set_text(StringHelp::int_to_text(anim.end_frame));
		play_property->text_field->set_text(StringHelp::float_to_text(anim.play_speed));
		move_property->text_field->set_text(StringHelp::float_to_text(anim.move_speed));
		loop_property->text_field->set_text(anim.loop ? "1" : "0");
		rarity_property->text_field->set_text(StringHelp::int_to_text(anim.rarity));
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
	auto selection = animations_list->selection();
	if (selection)
	{
		if (selection->index >= AppModel::instance()->desc.animations.size())
		{
			FBXAnimation anim;
			anim.name = selection->text();
			AppModel::instance()->undo_system.execute<AddAnimationCommand>(anim);

			animations_list->add_item("");
		}
		else
		{
			FBXAnimation anim = AppModel::instance()->desc.animations[selection->index];
			anim.name = selection->text();
			AppModel::instance()->undo_system.execute<UpdateAnimationCommand>(selection->index, anim);
		}

		update_animation_fields();
	}
}

void AnimationsController::start_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->desc.animations.at(selection->index);
		animation.start_frame = start_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsController::end_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->desc.animations.at(selection->index);
		animation.end_frame = end_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsController::play_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->desc.animations.at(selection->index);
		animation.play_speed = play_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsController::move_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->desc.animations.at(selection->index);
		animation.move_speed = move_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsController::loop_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->desc.animations.at(selection->index);
		animation.loop = loop_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsController::rarity_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->desc.animations.at(selection->index);
		animation.rarity = rarity_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}
