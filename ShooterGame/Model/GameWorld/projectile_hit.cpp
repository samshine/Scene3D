
#include "precomp.h"
#include "projectile_hit.h"
#include "collision_game_object.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

ProjectileHit::ProjectileHit(GameWorld *world, const uicore::Vec3f &init_pos, const uicore::Quaternionf &init_orientation) : GameObject(world), pos(init_pos), orientation(init_orientation)
{
	if (game_world()->client())
	{
		//auto model = SceneModel::create(game_world()->client()->scene(), "Models/Rocket_launcher/Bullet.cmodel");
		//scene_object = SceneObject::create(game_world()->client()->scene(), model, pos, orientation, Vec3f(4.0f));

		sound = AudioObject::create(game_world()->client()->audio());
		sound->set_sound("Sound/Weapons/grenade_bounce.ogg");
		sound->set_attenuation_begin(1.0f);
		sound->set_attenuation_end(100.0f);
		sound->set_volume(0.5f);
		sound->set_position(pos);
		sound->play();

		emitter = SceneParticleEmitter::create(game_world()->client()->scene());
		emitter->set_type(SceneParticleEmitter::type_spot);
		emitter->set_position(pos);
		emitter->set_orientation(Quaternionf::euler(radians(-90.0f), 0.0f, 0.0f));
		emitter->set_falloff(30.0f);
		emitter->set_particles_per_second(200.0f);
		emitter->set_life_span(0.5f);
		emitter->set_start_size(0.03f);
		emitter->set_end_size(0.10f);
		emitter->set_speed(2.0f);
		emitter->set_particle_texture("ParticleTextures/Smoke3/Smoke3.ctexture");
		emitter->set_gradient_texture("ParticleTextures/Smoke3/gradient.png");

		light = SceneLight::create(game_world()->client()->scene());
		light->set_type(SceneLight::type_omni);
		light->set_position(pos);
		light->set_attenuation_start(0.0f);
		light->set_attenuation_end(2.0f);
		light->set_color(Vec3f(1.0f, 0.7f, 0.5f));
	}
}

void ProjectileHit::tick()
{
	time_left = std::max(time_left - time_elapsed(), 0.0f);
	if (time_left == 0.0f)
	{
		remove();
		return;
	}
}

void ProjectileHit::frame()
{
	if (scene_object)
		scene_object->update(frame_time_elapsed());

	if (light)
		light->set_color(mix(Vec3f(1.0f, 0.7f, 0.5f), Vec3f(0.0f, 0.0f, 0.0f), time_left * 2.0f));
}
