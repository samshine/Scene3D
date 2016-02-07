
#pragma once

#include "Model/ClientWorld/client_world.h"

class Elevator : public GameObject, public ClientObject
{
public:
	Elevator(const uicore::Vec3f &pos1, const uicore::Vec3f &pos2, const uicore::Quaternionf &orientation, const std::string &model_name, float scale);
	~Elevator();

	void tick() override;

	int level_obj_id;

protected:
	void on_elevator_update(const std::string &sender, const uicore::JsonValue &message);

	void tick_down();
	void tick_start_triggered();
	void tick_moving_up();
	void tick_up();
	void tick_moving_down();

	bool test_start_trigger();

	void send_net_update(const std::string &target);

	uicore::Vec3f pos1, pos2;
	uicore::Quaternionf orientation;

	uicore::Vec3f box_size = uicore::Vec3f(1.118f, 0.2f, 1.436f); // Note: this size actually only fits for middle elevators
	Physics3DShapePtr box_shape;
	Physics3DObjectPtr body;

	SceneObjectPtr scene_object;

	enum State
	{
		state_down,
		state_start_triggered,
		state_moving_up,
		state_up,
		state_moving_down
	};

	State state = state_down;

	float time = 0.0f;
	float speed = 1.0f;
	float wait_time = 5.0f;

	std::shared_ptr<ModelData> create_box();
};
