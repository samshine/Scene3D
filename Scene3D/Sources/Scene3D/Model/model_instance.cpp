/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#include "precomp.h"
#include "model_instance.h"
#include "model.h"
#include "Scene3D/ModelData/model_data.h"
#include <algorithm>

namespace clan
{


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

		const auto &animation = renderer->get_model_data()->animations[animation_index];
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

		const auto &animation = renderer->get_model_data()->animations[animation_index];
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
		for (size_t i = 0; i < renderer->get_model_data()->animations.size(); i++)
		{
			if (renderer->get_model_data()->animations[i].name == animation_name)
			{
				if (renderer->get_model_data()->animations[i].rarity != 0xffff && playback_rarity == -1)
					playback_rarity = rand() * 0xffff / RAND_MAX;

				if (playback_rarity <= renderer->get_model_data()->animations[i].rarity)
				{
					animation_index = i;
					break;
				}
				else
				{
					playback_rarity -= renderer->get_model_data()->animations[i].rarity;
				}
			}
		}
	}
}

}

