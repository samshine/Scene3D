
#include "precomp.h"
#include "animations_dockable.h"
#include "Views/MainWindow/Dock/Rollout/rollout_view.h"
#include "Views/MainWindow/Dock/Rollout/rollout_list.h"
#include "Views/MainWindow/Dock/Rollout/rollout_text_field_property.h"
#include "Model/app_model.h"

using namespace clan;

AnimationsDockable::AnimationsDockable()
{
	style.set_layout_block();

	animations = std::make_shared<RolloutView>("Animations");
	animation = std::make_shared<RolloutView>("Animation");

	add_subview(animations);
	add_subview(animation);

	animations_list = std::make_shared<RolloutList>();
	start_property = std::make_shared<RolloutTextFieldProperty>("Start frame");
	end_property = std::make_shared<RolloutTextFieldProperty>("End frame");
	play_property = std::make_shared<RolloutTextFieldProperty>("Play speed");
	move_property = std::make_shared<RolloutTextFieldProperty>("Move speed");
	loop_property = std::make_shared<RolloutTextFieldProperty>("Loop");
	rarity_property = std::make_shared<RolloutTextFieldProperty>("Rarity");

	animations->content->add_subview(animations_list);

	animation->content->add_subview(start_property);
	animation->content->add_subview(end_property);
	animation->content->add_subview(play_property);
	animation->content->add_subview(move_property);
	animation->content->add_subview(loop_property);
	animation->content->add_subview(rarity_property);

	slots.connect(animations_list->sig_selection_changed(), this, &AnimationsDockable::animations_list_selection_changed);
	slots.connect(animations_list->sig_edit_saved(), this, &AnimationsDockable::animations_list_edit_saved);
	slots.connect(start_property->sig_value_changed(), this, &AnimationsDockable::start_property_value_changed);
	slots.connect(end_property->sig_value_changed(), this, &AnimationsDockable::end_property_value_changed);
	slots.connect(play_property->sig_value_changed(), this, &AnimationsDockable::play_property_value_changed);
	slots.connect(move_property->sig_value_changed(), this, &AnimationsDockable::move_property_value_changed);
	slots.connect(loop_property->sig_value_changed(), this, &AnimationsDockable::loop_property_value_changed);
	slots.connect(rarity_property->sig_value_changed(), this, &AnimationsDockable::rarity_property_value_changed);

	animations_list->add_animation("default")->set_selected(true, false);
	animations_list->add_animation("");
}

void AnimationsDockable::update_animation_fields()
{
	auto selection = animations_list->selection();
	if (selection && selection->index < AppModel::instance()->animations.size())
	{
		animation->set_hidden(false);

		const auto &anim = AppModel::instance()->animations[selection->index];
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

void AnimationsDockable::animations_list_selection_changed()
{
	update_animation_fields();
}

void AnimationsDockable::animations_list_edit_saved()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		if (selection->index >= AppModel::instance()->animations.size())
		{
			AppModel::instance()->animations.resize(selection->index + 1);
			animations_list->add_animation("");
		}

		auto &anim = AppModel::instance()->animations[selection->index];
		anim.name = selection->text();

		update_animation_fields();
	}
}

void AnimationsDockable::start_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->animations.at(selection->index);
		animation.start_frame = start_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsDockable::end_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->animations.at(selection->index);
		animation.end_frame = end_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsDockable::play_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->animations.at(selection->index);
		animation.play_speed = play_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsDockable::move_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->animations.at(selection->index);
		animation.move_speed = move_property->text_field->text_float();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsDockable::loop_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->animations.at(selection->index);
		animation.loop = loop_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

void AnimationsDockable::rarity_property_value_changed()
{
	auto selection = animations_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto animation = app_model->animations.at(selection->index);
		animation.rarity = rarity_property->text_field->text_int();
		app_model->undo_system.execute<UpdateAnimationCommand>(selection->index, animation);
	}
}

/*
Tpose 0 - 0
static 4 4
walk 32 83
run 115 144
*/
