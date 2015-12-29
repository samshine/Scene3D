
#include "precomp.h"
#include "rocket.h"
#include "game_world.h"
#include "collision_game_object.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

Rocket::Rocket(PlayerPawn *owner, const uicore::Vec3f &init_pos, const uicore::Quaternionf &init_orientation) : GameObject(owner->world()), owner(owner), pos(init_pos), orientation(init_orientation)
{
	last_pos = pos;
	last_orientation = orientation;

	if (world()->client)
	{
		auto model = SceneModel::create(world()->client->scene, "Models/Rocket_launcher/Bullet.cmodel");
		scene_object = SceneObject::create(world()->client->scene, model, pos, orientation, Vec3f(4.0f));

		sound = AudioObject::create(world()->client->audio);
		sound->set_sound("Sound/Weapons/grenade_launch.ogg");
		sound->set_attenuation_begin(1.0f);
		sound->set_attenuation_end(100.0f);
		sound->set_volume(0.5f);
		sound->set_position(pos);
		sound->play();

		emitter = SceneParticleEmitter::create(world()->client->scene);
		emitter->set_type(SceneParticleEmitter::type_omni);
		emitter->set_position(pos);
		emitter->set_orientation(orientation);
		emitter->set_particles_per_second(75.0f);
		emitter->set_life_span(0.75f);
		emitter->set_start_size(0.03f);
		emitter->set_end_size(2.0f);
		emitter->set_speed(1.0f);
		emitter->set_particle_texture("ParticleTextures/Smoke3/Smoke3.ctexture");
		emitter->set_gradient_texture("ParticleTextures/Smoke3/gradient.png");
	}
}

void Rocket::tick(const GameTick &tick)
{
	last_pos = pos;
	last_orientation = orientation;

	time_left = std::max(time_left - tick.time_elapsed, 0.0f);
	if (time_left == 0.0f)
	{
		world()->remove(this);
		return;
	}

	float speed = 40.0f;
	auto velocity = orientation.rotate_vector(Vec3f(0.0f, 0.0f, speed));

	pos += velocity * tick.time_elapsed;

	auto ray_hit = world()->collision->ray_test_nearest(last_pos, pos, [&](const Physics3DHit &result)
	{
		return result.object->data_object() != owner;
	});

	if (ray_hit)
	{
		PlayerPawn *pawn = ray_hit.object->data<PlayerPawn>();
		if (pawn)
		{
			pawn->apply_damage(tick, 100.0f);
		}
		world()->remove(this);
	}
}

void Rocket::frame(float time_elapsed, float interpolated_time)
{
	if (scene_object)
	{
		scene_object->set_position(mix(last_pos, pos, interpolated_time));
		scene_object->set_orientation(Quaternionf::lerp(last_orientation, orientation, interpolated_time));
		scene_object->update(time_elapsed);
	}

	if (emitter)
	{
		emitter->set_position(mix(last_pos, pos, interpolated_time));
		emitter->set_orientation(Quaternionf::lerp(last_orientation, orientation, interpolated_time));
	}

	if (sound)
		sound->set_position(mix(last_pos, pos, interpolated_time));
}
