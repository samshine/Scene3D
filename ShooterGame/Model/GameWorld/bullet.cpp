
#include "precomp.h"
#include "bullet.h"
#include "explosion.h"
#include "game_world.h"
#include "game_object_collision.h"
#include "player_pawn.h"
#include "Model/game.h"
#include <algorithm>

using namespace clan;

Bullet::Bullet(GameWorld *world, const std::string &type, const clan::Vec3f &init_pos, const clan::Quaternionf &init_orientation)
	: GameObject(world), pos(init_pos), orientation(init_orientation)
{
	ray_test = Physics3DRayTest(world->game()->collision);

	JsonValue desc = world->weapon_data["bullets"][type];

	time_left = desc["lifespan"].to_float();
	float speed = desc["speed"].to_float();
	velocity = orientation.rotate_vector(Vec3f(0.0f, 0.0f, speed));
	damage = desc["damage"].to_float();

	explosion_type = desc["explosion"].to_string();

	std::string model_name = desc["mesh"].to_string();
	float scale = desc["scale"].to_float();

	last_pos = pos;
	last_orientation = orientation;

	if (!world->is_server)
	{
		SceneModel model(world->game()->gc, *world->game()->scene, model_name);
		scene_object = SceneObject(*world->game()->scene, model, pos, orientation, Vec3f(scale));

		if (desc.get_members().find("fireSound") != desc.get_members().end())
		{
			sound = AudioObject(*world->game()->audio.get());
			sound.set_sound(desc["fireSound"]["sample"].to_string());
			sound.set_attenuation_begin(desc["fireSound"]["attenuationBegin"].to_float());
			sound.set_attenuation_end(desc["fireSound"]["attenuationEnd"].to_float());
			sound.set_volume(desc["fireSound"]["volume"].to_float());
			sound.set_position(pos);
			sound.play();
		}

		if (desc.get_members().find("particleEmitter") != desc.get_members().end())
		{
			emitter = SceneParticleEmitter(*world->game()->scene);
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
}

Bullet::~Bullet()
{
}

void Bullet::tick(const GameTick &tick)
{
	last_pos = pos;
	last_orientation = orientation;

	time_left = std::max(time_left - tick.time_elapsed, 0.0f);
	if (time_left == 0.0f)
	{
		world()->remove(this);
		return;
	}

	pos += velocity * tick.time_elapsed;

	velocity.y -= tick.time_elapsed * gravity;

	if (ray_test.test(last_pos, pos))
	{
		std::shared_ptr<GameObjectCollision> obj_collision = ray_test.get_hit_object().get_data<GameObjectCollision>();
		if (obj_collision && dynamic_cast<PlayerPawn*>(obj_collision->obj))
		{
			static_cast<PlayerPawn*>(obj_collision->obj)->apply_damage(tick, damage);
		}

		if (bounce)
		{
			/*
			var speed = Math.sqrt(this.velocityX * this.velocityX + this.velocityY * this.velocityY + this.velocityZ * this.velocityZ);
			if (speed != 0) {
				this.velocityX /= speed;
				this.velocityY /= speed;
				this.velocityZ /= speed;
			}

			// Reflect velocity direction on face normal
			var NdotI = this.velocityX * result.hitNormalX + this.velocityY * result.hitNormalY + this.velocityZ * result.hitNormalZ;
			this.velocityX -= 2 * NdotI * result.hitNormalX;
			this.velocityY -= 2 * NdotI * result.hitNormalY;
			this.velocityZ -= 2 * NdotI * result.hitNormalZ;

			this.velocityX *= speed;
			this.velocityY *= speed;
			this.velocityZ *= speed;

			// Energy loss when we bounce
			var energyLoss = this.objectDescription.bounce.energyLoss;
			this.velocityX *= energyLoss;
			this.velocityY *= energyLoss;
			this.velocityZ *= energyLoss;

			// Only play sound if we have high enough speed to justify it
			if (speed > this.objectDescription.bounce.minSpeedForSound) {
				var sound = new AudioObject(this.objectDescription.bounce.sound.sample);
				sound.setPosition(this.pivot.positionX, this.pivot.positionY, this.pivot.positionZ);
				sound.setAttenuationBegin(this.objectDescription.bounce.sound.attenuationBegin);
				sound.setAttenuationEnd(this.objectDescription.bounce.sound.attenuationEnd);
				sound.setVolume(this.objectDescription.bounce.sound.volume);
				sound.play();
				sound.dispose();
			}
			*/
		}
		else
		{
			world()->add(new Explosion(world(), explosion_type, last_pos, last_orientation));
			world()->remove(this);
			return;
		}
	}
}

void Bullet::frame(float time_elapsed, float interpolated_time)
{
	if (!scene_object.is_null())
	{
		scene_object.set_position(mix(last_pos, pos, interpolated_time));
		scene_object.set_orientation(Quaternionf::lerp(last_orientation, orientation, interpolated_time));
		scene_object.update(time_elapsed);
	}

	if (!emitter.is_null())
	{
		emitter.set_position(mix(last_pos, pos, interpolated_time));
		emitter.set_orientation(Quaternionf::lerp(last_orientation, orientation, interpolated_time));
	}

	if (!sound.is_null())
		scene_object.set_position(mix(last_pos, pos, interpolated_time));
}