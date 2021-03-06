
#include "precomp.h"
#include "model_instance.h"
#include "model.h"
#include "Scene3D/ModelData/model_data.h"
#include <algorithm>

using namespace uicore;

ModelInstance::ModelInstance()
{
}

void ModelInstance::set_renderer(std::shared_ptr<Model> new_renderer)
{
	renderer = new_renderer;
	last_anim.update_animation_index(renderer);
	cur_anim.update_animation_index(renderer);
}

void ModelInstance::play_animation(const std::string &name, bool instant)
{
	play_transition(name, std::string(), instant);
}

void ModelInstance::play_transition(const std::string &anim1, const std::string &anim2, bool instant)
{
	last_anim = cur_anim;
	cur_anim.play_animation(renderer, anim1, anim2, true);
	transition = instant ? 1.0f : 0.0f;
}

void ModelInstance::update(float time_elapsed)
{
	last_anim.update(renderer, time_elapsed);
	cur_anim.update(renderer, time_elapsed);
	transition = std::min(transition + time_elapsed * (1.0f / 0.150f), 1.0f);
}

void ModelInstance::moved(float units_moved)
{
	last_anim.moved(renderer, units_moved);
	cur_anim.moved(renderer, units_moved);
}

void ModelInstance::get_attachment_location(const std::string &name, Vec3f &out_position, Quaternionf &out_orientation) const
{
	out_position = Vec3f();
	out_orientation = Quaternionf();

	if (renderer)
	{
		const auto &model_data = renderer->model_data;
		for (const auto &attachment : model_data->attachment_points)
		{
			if (attachment.name == name)
			{
				const auto &bone = model_data->bones[attachment.bone_selector];

				Vec3f bone_position;
				Quaternionf bone_orientation;
				get_bone_transform(attachment.bone_selector, bone, bone_position, bone_orientation);

				Mat4f transform = Mat4f::translate(bone_position) * bone_orientation.to_matrix();

				Vec4f point_in_object = transform * Vec4f(attachment.position, 1.0f);
				Quaternionf orientation_in_object = bone_orientation * attachment.orientation;

				out_position = Vec3f(point_in_object);
				out_orientation = orientation_in_object;
				return;
			}
		}
	}
}

void ModelInstance::get_bone_transform(size_t bone_index, const ModelDataBone &bone, Vec3f &position, Quaternionf &orientation) const
{
	if (!renderer)
	{
		position = Vec3f();
		orientation = Quaternionf();
		return;
	}

	if (ragdoll_bone_positions.size() > bone_index && ragdoll_bone_orientations.size() > bone_index)
	{
		position = ragdoll_bone_positions[bone_index];
		orientation = ragdoll_bone_orientations[bone_index];
		return;
	}

	const auto &model_data = renderer->model_data;

	int animation_index = cur_anim.get_animation_index();
	float animation_time = cur_anim.get_animation_time();

	int last_animation_index = last_anim.get_animation_index();
	float last_animation_time = last_anim.get_animation_time();

	position = bone.position.get_value(animation_index, animation_time);
	orientation = bone.orientation.get_value(animation_index, animation_time);

	if (transition < 1.0f && last_animation_index != -1)
	{
		Vec3f last_position = bone.position.get_value(last_animation_index, last_animation_time);
		Quaternionf last_orientation = bone.orientation.get_value(last_animation_index, last_animation_time);

		position = mix(last_position, position, transition);
		orientation = Quaternionf::slerp(last_orientation, orientation, transition);
	}
}

/////////////////////////////////////////////////////////////////////////////

void ModelAnimationTime::play_animation(std::shared_ptr<Model> &renderer, const std::string &anim1, const std::string &anim2, bool instant)
{
	next_animation_name = anim1;
	next_animation_name2 = anim2;
	if (instant || animation_index == -1)
		next_animation(renderer);
}

void ModelAnimationTime::next_animation(std::shared_ptr<Model> &renderer)
{
	animation_name = next_animation_name;
	animation_time = 0.0f;
	next_animation_name = next_animation_name2;
	next_animation_name2.clear();
	if (animation_name.empty())
		animation_name = "default";
	update_animation_index(renderer);
}

void ModelAnimationTime::update(std::shared_ptr<Model> &renderer, float time_elapsed)
{
	while (time_elapsed > 0.0f)
	{
		if (animation_index == -1)
			break;

		const auto &animation = renderer->model_data->animations[animation_index];
		if (animation.playback_speed <= 0.0f || animation.length <= 0.0f)
		{
			next_animation(renderer);
			break;
		}
		else
		{
			float time_left = (animation.length - animation_time) / animation.playback_speed;
			if (time_left < time_elapsed)
			{
				time_elapsed -= time_left;
				animation_time = 0.0f;

				if (!animation.loop || !next_animation_name.empty())
					next_animation(renderer);
			}
			else
			{
				animation_time += time_elapsed * animation.playback_speed;
				break;
			}
		}
	}
}

void ModelAnimationTime::moved(std::shared_ptr<Model> &renderer, float units_moved)
{
	while (units_moved > 0.0f)
	{
		if (animation_index == -1)
			break;

		const auto &animation = renderer->model_data->animations[animation_index];
		if (animation.moving_speed <= 0.0f || animation.length <= 0.0f)
			break;

		float units_left = (animation.length - animation_time) / animation.moving_speed;
		if (units_left < units_moved)
		{
			units_moved -= units_left;
			animation_time = 0.0f;

			if (!animation.loop || !next_animation_name.empty())
				next_animation(renderer);
		}
		else
		{
			animation_time += units_moved * animation.moving_speed;
			break;
		}
	}
}

void ModelAnimationTime::update_animation_index(std::shared_ptr<Model> &renderer)
{
	animation_index = -1;
	if (renderer)
	{
		int playback_rarity = -1;
		for (size_t i = 0; i < renderer->model_data->animations.size(); i++)
		{
			if (renderer->model_data->animations[i].name == animation_name)
			{
				if (renderer->model_data->animations[i].rarity != 0xffff && playback_rarity == -1)
					playback_rarity = rand() * 0xffff / RAND_MAX;

				if (playback_rarity <= renderer->model_data->animations[i].rarity)
				{
					animation_index = i;
					break;
				}
				else
				{
					playback_rarity -= renderer->model_data->animations[i].rarity;
				}
			}
		}

		if (animation_index == -1 && !renderer->model_data->animations.empty())
			animation_index = 0;
	}
}
