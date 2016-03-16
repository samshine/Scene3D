
#include "precomp.h"
#include "environment_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Model/ModelEditor/model_app_model.h"
#include "Model/ModelEditor/Commands/set_map_model_command.h"

using namespace uicore;

EnvironmentController::EnvironmentController()
{
	environment = std::make_shared<RolloutView>("ENVIRONMENT");
	map_model_property = std::make_shared<RolloutBrowseFieldProperty>("MAP MODEL");

	environment->content->add_child(map_model_property);

	character = std::make_shared<RolloutView>("CHARACTER CONTROLLER");
	gravity_property = std::make_shared<RolloutTextFieldProperty>("GRAVITY");
	height_property = std::make_shared<RolloutTextFieldProperty>("HEIGHT");
	radius_property = std::make_shared<RolloutTextFieldProperty>("RADIUS");
	step_height_property = std::make_shared<RolloutTextFieldProperty>("STEP HEIGHT");
	mass_property = std::make_shared<RolloutTextFieldProperty>("MASS");
	acceleration_property = std::make_shared<RolloutTextFieldProperty>("ACCELERATION");
	run_speed_property = std::make_shared<RolloutTextFieldProperty>("RUN SPEED");
	friction_property = std::make_shared<RolloutTextFieldProperty>("FRICTION");
	air_resistance_property = std::make_shared<RolloutTextFieldProperty>("AIR RESISTANCE");
	air_movement_property = std::make_shared<RolloutTextFieldProperty>("AIR MOVEMENT");
	bounce_property = std::make_shared<RolloutTextFieldProperty>("BOUNCE");

	character->content->add_child(gravity_property);
	character->content->add_child(height_property);
	character->content->add_child(radius_property);
	character->content->add_child(step_height_property);
	character->content->add_child(mass_property);
	character->content->add_child(acceleration_property);
	character->content->add_child(run_speed_property);
	character->content->add_child(friction_property);
	character->content->add_child(air_resistance_property);
	character->content->add_child(air_movement_property);
	character->content->add_child(bounce_property);

	view->content_view()->add_child(environment);
	view->content_view()->add_child(character);

	slots.connect(ModelAppModel::instance()->sig_map_model_updated, this, &EnvironmentController::map_model_updated);
	slots.connect(map_model_property->sig_browse(), this, &EnvironmentController::map_model_property_browse);
	slots.connect(gravity_property->sig_value_changed(), this, &EnvironmentController::gravity_property_value_changed);
	slots.connect(height_property->sig_value_changed(), this, &EnvironmentController::height_property_value_changed);
	slots.connect(radius_property->sig_value_changed(), this, &EnvironmentController::radius_property_value_changed);
	slots.connect(step_height_property->sig_value_changed(), this, &EnvironmentController::step_height_property_value_changed);
	slots.connect(mass_property->sig_value_changed(), this, &EnvironmentController::mass_property_value_changed);
	slots.connect(acceleration_property->sig_value_changed(), this, &EnvironmentController::acceleration_property_value_changed);
	slots.connect(run_speed_property->sig_value_changed(), this, &EnvironmentController::run_speed_property_value_changed);
	slots.connect(friction_property->sig_value_changed(), this, &EnvironmentController::friction_property_value_changed);
	slots.connect(air_resistance_property->sig_value_changed(), this, &EnvironmentController::air_resistance_property_value_changed);
	slots.connect(air_movement_property->sig_value_changed(), this, &EnvironmentController::air_movement_property_value_changed);
	slots.connect(bounce_property->sig_value_changed(), this, &EnvironmentController::bounce_property_value_changed);
	/*
	CharacterController *cc = ModelAppModel::instance()->editor_scene->get_character_controller();
	if (cc)
	{
		gravity_property->text_field->set_text(Text::to_string(cc->get_gravity()));
		height_property->text_field->set_text(Text::to_string(cc->get_height()));
		radius_property->text_field->set_text(Text::to_string(cc->get_radius()));
		step_height_property->text_field->set_text(Text::to_string(cc->get_step_height()));
		mass_property->text_field->set_text(Text::to_string(cc->get_mass()));
		acceleration_property->text_field->set_text(Text::to_string(cc->get_acceleration()));
		run_speed_property->text_field->set_text(Text::to_string(cc->get_run_speed()));
		friction_property->text_field->set_text(Text::to_string(cc->get_friction()));
		air_resistance_property->text_field->set_text(Text::to_string(cc->get_air_resistance()));
		air_movement_property->text_field->set_text(Text::to_string(cc->get_air_movement()));
		bounce_property->text_field->set_text(Text::to_string(cc->get_bounce()));
	}
	else
	{
		character->set_hidden(true);
	}
	*/
	map_model_updated();
}

void EnvironmentController::map_model_updated()
{
	map_model_property->browse_field->set_text(PathHelp::basename(ModelAppModel::instance()->map_model));
}

void EnvironmentController::map_model_property_browse()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Map Model");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(ModelAppModel::instance()->map_model);
	if (dialog.show())
	{
		ModelAppModel::instance()->undo_system.execute<SetMapModelCommand>(dialog.filename());
	}
}

void EnvironmentController::gravity_property_value_changed()
{
	//float gravity = gravity_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_gravity(gravity);
}

void EnvironmentController::height_property_value_changed()
{
	//float height = height_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_height(height);
}

void EnvironmentController::radius_property_value_changed()
{
	//float radius = radius_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_radius(radius);
}

void EnvironmentController::step_height_property_value_changed()
{
	//float step_height = step_height_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_step_height(step_height);
}

void EnvironmentController::mass_property_value_changed()
{
	//float mass = mass_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_mass(mass);
}

void EnvironmentController::acceleration_property_value_changed()
{
	//float acceleration = acceleration_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_acceleration(acceleration);
}

void EnvironmentController::run_speed_property_value_changed()
{
	//float run_speed = run_speed_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_run_speed(run_speed);
}

void EnvironmentController::friction_property_value_changed()
{
	//float friction = friction_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_friction(friction);
}

void EnvironmentController::air_resistance_property_value_changed()
{
	//float air_resistance = air_resistance_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_air_resistance(air_resistance);
}

void EnvironmentController::air_movement_property_value_changed()
{
	//float air_movement = air_movement_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_air_movement(air_movement);
}

void EnvironmentController::bounce_property_value_changed()
{
	//float bounce = bounce_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_bounce(bounce);
}
