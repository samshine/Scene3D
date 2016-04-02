
#include "precomp.h"
#include "environment_controller.h"
#include "Views/ModelEditor/environment_sidepanel.h"
#include "Model/ModelEditor/model_app_model.h"
#include "Model/ModelEditor/Commands/set_map_model_command.h"

using namespace uicore;

EnvironmentController::EnvironmentController()
{
	panel = view->content_view()->add_child<EnvironmentSidePanelView>();

	slots.connect(ModelAppModel::instance()->sig_map_model_updated, this, &EnvironmentController::map_model_updated);
	slots.connect(panel->map_model_property->sig_browse(), this, &EnvironmentController::map_model_property_browse);
	slots.connect(panel->gravity_property->sig_value_changed(), this, &EnvironmentController::gravity_property_value_changed);
	slots.connect(panel->height_property->sig_value_changed(), this, &EnvironmentController::height_property_value_changed);
	slots.connect(panel->radius_property->sig_value_changed(), this, &EnvironmentController::radius_property_value_changed);
	slots.connect(panel->step_height_property->sig_value_changed(), this, &EnvironmentController::step_height_property_value_changed);
	slots.connect(panel->mass_property->sig_value_changed(), this, &EnvironmentController::mass_property_value_changed);
	slots.connect(panel->acceleration_property->sig_value_changed(), this, &EnvironmentController::acceleration_property_value_changed);
	slots.connect(panel->run_speed_property->sig_value_changed(), this, &EnvironmentController::run_speed_property_value_changed);
	slots.connect(panel->friction_property->sig_value_changed(), this, &EnvironmentController::friction_property_value_changed);
	slots.connect(panel->air_resistance_property->sig_value_changed(), this, &EnvironmentController::air_resistance_property_value_changed);
	slots.connect(panel->air_movement_property->sig_value_changed(), this, &EnvironmentController::air_movement_property_value_changed);
	slots.connect(panel->bounce_property->sig_value_changed(), this, &EnvironmentController::bounce_property_value_changed);
	/*
	CharacterController *cc = ModelAppModel::instance()->editor_scene->get_character_controller();
	if (cc)
	{
		panel->gravity_property->text_field->set_text(Text::to_string(cc->get_gravity()));
		panel->height_property->text_field->set_text(Text::to_string(cc->get_height()));
		panel->radius_property->text_field->set_text(Text::to_string(cc->get_radius()));
		panel->step_height_property->text_field->set_text(Text::to_string(cc->get_step_height()));
		panel->mass_property->text_field->set_text(Text::to_string(cc->get_mass()));
		panel->acceleration_property->text_field->set_text(Text::to_string(cc->get_acceleration()));
		panel->run_speed_property->text_field->set_text(Text::to_string(cc->get_run_speed()));
		panel->friction_property->text_field->set_text(Text::to_string(cc->get_friction()));
		panel->air_resistance_property->text_field->set_text(Text::to_string(cc->get_air_resistance()));
		panel->air_movement_property->text_field->set_text(Text::to_string(cc->get_air_movement()));
		panel->bounce_property->text_field->set_text(Text::to_string(cc->get_bounce()));
	}
	else
	{
		panel->character->set_hidden(true);
	}
	*/
	map_model_updated();
}

void EnvironmentController::map_model_updated()
{
	panel->map_model_property->browse_field->set_text(PathHelp::basename(ModelAppModel::instance()->map_model));
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
	//float gravity = panel->gravity_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_gravity(gravity);
}

void EnvironmentController::height_property_value_changed()
{
	//float height = panel->height_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_height(height);
}

void EnvironmentController::radius_property_value_changed()
{
	//float radius = panel->radius_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_radius(radius);
}

void EnvironmentController::step_height_property_value_changed()
{
	//float step_height = panel->step_height_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_step_height(step_height);
}

void EnvironmentController::mass_property_value_changed()
{
	//float mass = panel->mass_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_mass(mass);
}

void EnvironmentController::acceleration_property_value_changed()
{
	//float acceleration = panel->acceleration_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_acceleration(acceleration);
}

void EnvironmentController::run_speed_property_value_changed()
{
	//float run_speed = panel->run_speed_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_run_speed(run_speed);
}

void EnvironmentController::friction_property_value_changed()
{
	//float friction = panel->friction_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_friction(friction);
}

void EnvironmentController::air_resistance_property_value_changed()
{
	//float air_resistance = panel->air_resistance_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_air_resistance(air_resistance);
}

void EnvironmentController::air_movement_property_value_changed()
{
	//float air_movement = panel->air_movement_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_air_movement(air_movement);
}

void EnvironmentController::bounce_property_value_changed()
{
	//float bounce = panel->bounce_property->text_field->text_float();
	//ModelAppModel::instance()->editor_scene->get_character_controller()->set_bounce(bounce);
}
