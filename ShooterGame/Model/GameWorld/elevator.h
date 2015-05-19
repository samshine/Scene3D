
#pragma once

#include "game_object.h"

class Elevator : public GameObject
{
public:
	Elevator(GameWorld *world, int level_obj_id, const clan::Vec3f &pos1, const clan::Vec3f &pos2, const clan::Quaternionf &orientation, const std::string &model_name, float scale = 0.1f);
	~Elevator();

	void tick(const GameTick &tick) override;

	void net_update(const GameTick &tick, const clan::NetGameEvent &net_event);

	void send_net_update(const GameTick &tick, const std::string &target);

	int level_obj_id;

protected:
	void tick_down(const GameTick &tick);
	void tick_start_triggered(const GameTick &tick);
	void tick_moving_up(const GameTick &tick);
	void tick_up(const GameTick &tick);
	void tick_moving_down(const GameTick &tick);

	bool test_start_trigger();

	clan::Vec3f pos1, pos2;
	clan::Quaternionf orientation;

	clan::Vec3f box_size = clan::Vec3f(7.0f, 0.2f, 9.0f);
	clan::Physics3DShape box_shape;
	clan::Physics3DObject body;

	clan::SceneObject scene_object;

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

	std::shared_ptr<clan::ModelData> create_box();
};
