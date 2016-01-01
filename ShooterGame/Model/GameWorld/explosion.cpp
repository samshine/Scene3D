
#include "precomp.h"
#include "explosion.h"
#include "game_world.h"
#include "collision_game_object.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

Explosion::Explosion(GameWorld *world, const std::string &type, const uicore::Vec3f &init_pos, const uicore::Quaternionf &init_orientation)
: GameObject(world), pos(init_pos), orientation(init_orientation)
{
	JsonValue desc = world->weapon_data["explosions"][type];

	time_left = desc["lifespan"].to_float();

	if (world->client)
	{
		if (!desc["model"].is_undefined())
		{
			std::string model_name = desc["model"]["mesh"].to_string();
			float scale = desc["model"]["scale"].to_float();
			Vec3f offset(desc["model"]["offset"]["x"].to_float(), desc["model"]["offset"]["y"].to_float(), desc["model"]["offset"]["z"].to_float());

			auto model = SceneModel::create(world->client->scene, model_name);
			scene_object = SceneObject::create(world->client->scene, model, pos + orientation.rotate_vector(offset), orientation, Vec3f(scale));
		}

		if (!desc["sound"].is_undefined())
		{
			sound = AudioObject::create(world->client->audio);
			sound->set_sound(desc["sound"]["sample"].to_string());
			sound->set_attenuation_begin(desc["sound"]["attenuationBegin"].to_float());
			sound->set_attenuation_end(desc["sound"]["attenuationEnd"].to_float());
			sound->set_volume(desc["sound"]["volume"].to_float());
			sound->set_position(pos);
			sound->play();
		}

		if (!desc["particleEmitter"].is_undefined())
		{
			emitter = SceneParticleEmitter::create(world->client->scene);
			emitter->set_type(SceneParticleEmitter::type_omni);
			emitter->set_position(pos);
			emitter->set_orientation(orientation);
			emitter->set_particles_per_second(desc["particleEmitter"]["particlesPerSecond"].to_float());
			emitter->set_life_span(desc["particleEmitter"]["lifespan"].to_float());
			emitter->set_start_size(desc["particleEmitter"]["startSize"].to_float());
			emitter->set_end_size(desc["particleEmitter"]["endSize"].to_float());
			emitter->set_speed(desc["particleEmitter"]["speed"].to_float());
			emitter->set_particle_texture(desc["particleEmitter"]["particleTexture"].to_string());
			emitter->set_gradient_texture(desc["particleEmitter"]["gradientTexture"].to_string());
		}
	}

	float radius = 1.0f;
	float damage = 45.0f;

	// Check if we hit anyone nearby
	auto shape = Physics3DShape::sphere(radius);

	std::map<Physics3DObject *, bool> hits;
	for (const auto &contact : world->collision->contact_test_all(shape, pos, orientation))
		hits[contact.object] = true;

	for (auto it : hits)
	{
		PlayerPawn *pawn = it.first->data<PlayerPawn>();
		if (pawn)
		{
			float normalized_distance = std::min(pos.distance(pawn->get_position()) / radius, 1.0f);
			float player_damage = damage * (1.0f - normalized_distance);

			pawn->apply_damage(player_damage);

			//Console::write_line("normalized_distance = %1, player_damage = %2", normalized_distance, player_damage);
		}
	}
}

Explosion::~Explosion()
{
}

void Explosion::tick()
{
	time_left = std::max(time_left - time_elapsed(), 0.0f);
	if (time_left == 0.0f)
	{
		world()->remove(this);
		return;
	}
}

void Explosion::frame(float time_elapsed, float interpolated_time)
{
	if (scene_object)
	{
		scene_object->update(time_elapsed);
	}
}
