
#include "precomp.h"
#include "explosion.h"
#include "game_world.h"
#include "game_object_collision.h"
#include "player_pawn.h"
#include "Model/game.h"
#include <algorithm>

using namespace uicore;

Explosion::Explosion(GameWorld *world, const std::string &type, const uicore::Vec3f &init_pos, const uicore::Quaternionf &init_orientation)
: GameObject(world), pos(init_pos), orientation(init_orientation)
{
	JsonValue desc = world->weapon_data["explosions"][type];

	time_left = desc["lifespan"].to_float();

	if (!world->is_server)
	{
		if (!desc["model"].is_undefined())
		{
			std::string model_name = desc["model"]["mesh"].to_string();
			float scale = desc["model"]["scale"].to_float();
			Vec3f offset(desc["model"]["offset"]["x"].to_float(), desc["model"]["offset"]["y"].to_float(), desc["model"]["offset"]["z"].to_float());

			SceneModel model(world->game()->gc, world->game()->scene, model_name);
			scene_object = SceneObject(world->game()->scene, model, pos + orientation.rotate_vector(offset), orientation, Vec3f(scale));
		}

		if (!desc["sound"].is_undefined())
		{
			sound = AudioObject(*world->game()->audio.get());
			sound.set_sound(desc["sound"]["sample"].to_string());
			sound.set_attenuation_begin(desc["sound"]["attenuationBegin"].to_float());
			sound.set_attenuation_end(desc["sound"]["attenuationEnd"].to_float());
			sound.set_volume(desc["sound"]["volume"].to_float());
			sound.set_position(pos);
			sound.play();
		}

		if (!desc["particleEmitter"].is_undefined())
		{
			emitter = SceneParticleEmitter(world->game()->scene);
			emitter.set_type(SceneParticleEmitter::type_omni);
			emitter.set_position(pos);
			emitter.set_orientation(orientation);
			emitter.set_particles_per_second(desc["particleEmitter"]["particlesPerSecond"].to_float());
			emitter.set_life_span(desc["particleEmitter"]["lifespan"].to_float());
			emitter.set_start_size(desc["particleEmitter"]["startSize"].to_float());
			emitter.set_end_size(desc["particleEmitter"]["endSize"].to_float());
			emitter.set_speed(desc["particleEmitter"]["speed"].to_float());
			emitter.set_particle_texture(desc["particleEmitter"]["particleTexture"].to_string());
			emitter.set_gradient_texture(desc["particleEmitter"]["gradientTexture"].to_string());
		}
	}

	float radius = 30.0f;
	float damage = 45.0f;

	// Check if we hit anyone nearby
	Physics3DShape shape = Physics3DShape::sphere(radius);
	Physics3DObject sphere = Physics3DObject::collision_body(world->game()->collision, shape, pos, orientation);
	Physics3DContactTest test(world->game()->collision);
	test.test(sphere);
	for (int i = 0; i < test.get_hit_count(); i++)
	{
		Physics3DObject obj = test.get_hit_object(i);	
		std::shared_ptr<GameObjectCollision> obj_collision = obj.get_data<GameObjectCollision>();
		if (obj_collision && dynamic_cast<PlayerPawn*>(obj_collision->obj))
		{
			float normalized_distance = std::min(pos.distance(static_cast<PlayerPawn*>(obj_collision->obj)->get_position()) / radius, 1.0f);
			float player_damage = damage * (1.0f - normalized_distance);

			static_cast<PlayerPawn*>(obj_collision->obj)->apply_damage(world->net_tick, player_damage);

			//Console::write_line("normalized_distance = %1, player_damage = %2", normalized_distance, player_damage);
		}
	}

}

Explosion::~Explosion()
{
}

void Explosion::tick(const GameTick &tick)
{
	time_left = std::max(time_left - tick.time_elapsed, 0.0f);
	if (time_left == 0.0f)
	{
		world()->remove(this);
		return;
	}
}

void Explosion::frame(float time_elapsed, float interpolated_time)
{
	if (!scene_object.is_null())
	{
		scene_object.update(time_elapsed);
	}
}
