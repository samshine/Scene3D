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
: animation_name("default"), animation_index(-1), animation_time(0.0f)
{
}

void ModelInstance::set_renderer(std::shared_ptr<Model> new_renderer)
{
	renderer = new_renderer;
	update_animation_index();
}

void ModelInstance::play_animation(const std::string &name, bool instant)
{
	play_transition(name, std::string(), instant);
}

void ModelInstance::play_transition(const std::string &anim1, const std::string &anim2, bool instant)
{
	next_animation_name = anim1;
	next_animation_name2 = anim2;
	if (instant || animation_index == -1)
		next_animation();
}

void ModelInstance::next_animation()
{
	animation_name = next_animation_name;
	animation_time = 0.0f;
	next_animation_name = next_animation_name2;
	next_animation_name2.clear();
	if (animation_name.empty())
		animation_name = "default";
	update_animation_index();
}

void ModelInstance::update(float time_elapsed)
{
	while (time_elapsed > 0.0f)
	{
		if (animation_index == -1)
			break;

		const auto &animation = renderer->get_model_data()->animations[animation_index];
		if (animation.playback_speed <= 0.0f || animation.length <= 0.0f)
		{
			next_animation();
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
					next_animation();
			}
			else
			{
				animation_time += time_elapsed * animation.playback_speed;
				break;
			}
		}
	}
}

void ModelInstance::moved(float units_moved)
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
				next_animation();
		}
		else
		{
			animation_time += units_moved * animation.moving_speed;
			break;
		}
	}
}

void ModelInstance::update_animation_index()
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

