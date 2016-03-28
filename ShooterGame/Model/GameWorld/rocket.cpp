
#include "precomp.h"
#include "rocket.h"
#include "collision_game_object.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

Rocket::Rocket(GameWorld *world, PlayerPawn *owner, const uicore::Vec3f &init_pos, const uicore::Quaternionf &init_orientation) : GameObject(world), owner(owner), pos(init_pos), orientation(init_orientation)
{
	last_pos = pos;
	last_orientation = orientation;
}

Rocket::Rocket(GameWorld *world, const uicore::JsonValue &net_event) : GameObject(world)
{
	pos = Vec3f(net_event["x"].to_float(), net_event["y"].to_float(), net_event["z"].to_float());
	orientation = Quaternionf(net_event["rw"].to_float(), net_event["rx"].to_float(), net_event["ry"].to_float(), net_event["rz"].to_float());

	last_pos = pos;
	last_orientation = orientation;

	if (game_world()->client())
	{
		auto model = SceneModel::create(game_world()->client()->scene(), "Models/Rocket_launcher/Bullet.cmodel");
		scene_object = SceneObject::create(game_world()->client()->scene(), model, pos, orientation, Vec3f(4.0f));

		sound = AudioObject::create(game_world()->client()->audio());
		sound->set_sound("Sound/Weapons/grenade_launch.ogg");
		sound->set_attenuation_begin(1.0f);
		sound->set_attenuation_end(100.0f);
		sound->set_volume(0.5f);
		sound->set_position(pos);
		sound->play();

		emitter = SceneParticleEmitter::create(game_world()->client()->scene());
		emitter->set_type(SceneParticleEmitter::type_spot);
		emitter->set_position(pos);
		emitter->set_orientation(Quaternionf(-90.0f, 0.0f, 0.0f, angle_degrees, order_YXZ));
		emitter->set_falloff(30.0f);
		emitter->set_particles_per_second(25.0f);
		emitter->set_life_span(1.0f);
		emitter->set_start_size(0.03f);
		emitter->set_end_size(4.0f);
		emitter->set_speed(4.0f);
		emitter->set_particle_texture("ParticleTextures/Smoke3/Smoke3.ctexture");
		emitter->set_gradient_texture("ParticleTextures/Smoke3/gradient.png");
	}
}

void Rocket::send_create()
{
	JsonValue message = JsonValue::object();
	message["name"] = JsonValue::string("create");
	message["type"] = JsonValue::string("rocket");
	message["x"] = JsonValue::number(pos.x);
	message["y"] = JsonValue::number(pos.y);
	message["z"] = JsonValue::number(pos.z);
	message["rx"] = JsonValue::number(orientation.x);
	message["ry"] = JsonValue::number(orientation.y);
	message["rz"] = JsonValue::number(orientation.z);
	message["rw"] = JsonValue::number(orientation.w);
	send_event("all", message);
}

void Rocket::tick()
{
	last_pos = pos;
	last_orientation = orientation;

	time_left = std::max(time_left - time_elapsed(), 0.0f);
	if (time_left == 0.0f)
	{
		remove();
		return;
	}

	float speed = 40.0f;
	auto direction = orientation.rotate_vector(Vec3f(0.0f, 0.0f, 1.0f));

	pos += direction * (speed * time_elapsed());

	auto ray_hit = game_world()->kinematic_collision()->ray_test_nearest(last_pos, pos, [&](const Physics3DHit &result)
	{
		return owner ? result.object->data_object() != owner : true;
	});

	if (ray_hit)
	{
		Vec3f explosion_center = ray_hit.position - direction * 0.1f;

		float radius = 4.0f;
		float max_damage = 150.0f;
		float force = 1250.0f;

		if (!game_world()->client())
		{
			for (const auto &contact : game_world()->kinematic_collision()->contact_test_all(Physics3DShape::sphere(radius), explosion_center, Quaternionf()))
			{
				PlayerPawn *pawn = contact.object->data<PlayerPawn>();
				if (pawn)
				{
					auto pawn_center = pawn->get_position() + pawn->eye_offset;
					auto impulse_dir = pawn_center - explosion_center;
					auto dist = impulse_dir.length();

					if (dist < radius)
					{
						if (dist != 0.0f)
							impulse_dir *= 1.0f / dist;
						float impact = 1.0f - dist / radius;

						pawn->apply_damage(max_damage * impact);
						pawn->apply_impulse(impulse_dir * (impact * force));
					}
				}
			}
		}

		if (game_world()->client())
		{
			auto orientation = Quaternionf(Mat4f::look_at(Vec3f(0.0f), ray_hit.normal, std::abs(ray_hit.normal.y) > std::abs(ray_hit.normal.z) ? Vec3f(0.0f, 0.0f, 1.0f) : Vec3f(0.0f, 1.0f, 0.0f))).inverse();
			orientation = orientation * Quaternionf(0.0f, 0.0f, (rand() / (float)RAND_MAX) * 2 * PI, angle_radians, order_XYZ);

			auto decal = SceneDecal::create(game_world()->client()->scene());
			decal->set_position(ray_hit.position);
			decal->set_orientation(orientation);
			decal->set_extents(Vec3f(1.0f, 1.0f, 0.1f));
			decal->set_diffuse_texture(string_format("BulletHoles/bullethole%1.png", 1 + rand() % 4));
			/*int num = 1 + rand() % 73;
			if (num < 10)
				decal->set_diffuse_texture(string_format("Decal/Graffity/Graffity_00%1.png", num));
			else
				decal->set_diffuse_texture(string_format("Decal/Graffity/Graffity_0%1.png", num));*/
			game_world()->client()->decals().push_back(decal);
		}

		remove();
	}
}

void Rocket::frame()
{
	if (scene_object)
	{
		scene_object->set_position(mix(last_pos, pos, frame_interpolated_time()));
		scene_object->set_orientation(Quaternionf::lerp(last_orientation, orientation, frame_interpolated_time()));
		scene_object->update(frame_time_elapsed());
	}

	if (emitter)
	{
		emitter->set_position(mix(last_pos, pos, frame_interpolated_time()));
		//emitter->set_orientation(Quaternionf::lerp(last_orientation, orientation, frame_interpolated_time()));
	}

	if (sound)
		sound->set_position(mix(last_pos, pos, frame_interpolated_time()));
}
