
#include "precomp.h"
#include "player_pawn.h"
#include "game_world.h"
#include "game_tick.h"
#include "game_object_collision.h"
#include "Model/game.h"
#include <algorithm>

using namespace clan;

PlayerPawn::PlayerPawn(GameWorld *world)
: GameObject(world)
{
	controller.reset(new OldCharacterController(world->game()->collision, 1.0f, 2.89f, 0.5f, 58.0f));
	controller->get_object().set_data(std::make_shared<GameObjectCollision>(this));

	dir = 90.0f;
	up = 0.0f;
	tilt = 0.0f;

	weapon.reset(new Weapon(this));
}

PlayerPawn::~PlayerPawn()
{
}

void PlayerPawn::tick(const GameTick &tick)
{
	bool was_moving = animation_move_speed > 0.0f;

	Vec3f move_dir = tick_controller_movement(tick.time_elapsed);

	animation_move_speed = move_dir.length();

	bool is_moving = animation_move_speed > 0.0f;

	switch (key_weapon)
	{
	case 1: weapon->try_switch_to_weapon("IceLauncher"); break;
	case 2: weapon->try_switch_to_weapon("RocketLauncher"); break;
	case 3: weapon->try_switch_to_weapon("DualGuns"); break;
	case 4: weapon->try_switch_to_weapon("SniperRifle"); break;
	case 5: weapon->try_switch_to_weapon("TractorBeam"); break;
	}

	if (key_fire_primary)
		weapon->start_fire("primary");
	else
		weapon->stop_fire("primary");

	if (key_fire_secondary)
		weapon->start_fire("secondary");
	else
		weapon->stop_fire("secondary");

	weapon->tick(tick);
}

void PlayerPawn::ground_moved(const Vec3f &offset)
{
	controller->set_position(controller->get_position() + offset);
}

Vec3f PlayerPawn::tick_controller_movement(float time_elapsed)
{
	Vec3f move_circle;
	if (key_forward)
		move_circle.z += 1.0f;
	if (key_back)
		move_circle.z -= 1.0f;
	if (key_left)
		move_circle.x -= 1.0f;
	if (key_right)
		move_circle.x += 1.0f;
	move_circle.normalize();

	Vec3f move_oval = move_circle * Vec3f(slide_speed, 0.0f, move_speed); // Convert circle to an oval with the desired forward and slide speeds
	if (move_oval.z < 0.0f) // Move slightly slower backwards
		move_oval.z *= 0.8f;

	Vec3f move_dir = Quaternionf(0.0f, dir, 0.0f, angle_degrees, order_YXZ).rotate_vector(move_oval);

	move_velocity += move_dir * time_elapsed;
	move_velocity = move_velocity * move_resistance;

	if (Vec3f::dot(move_velocity, move_velocity) < 0.5f)
		move_velocity = Vec3f();

	controller->move(move_velocity, time_elapsed);
	if (key_jump)
		controller->jump(move_velocity + Vec3f(0.0f, 20.0f, 0.0f));

	return move_velocity;
}
