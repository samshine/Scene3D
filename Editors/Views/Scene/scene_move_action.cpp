
#include "precomp.h"
#include "scene_move_action.h"
#include "mouse_movement.h"

using namespace uicore;

SceneMoveAction::SceneMoveAction()
{
	timer->func_expired() = [this]()
	{
		gametime.update();
		func_key_repeat(keys, gametime.get_time_elapsed());
	};
}

void SceneMoveAction::track_keys(const std::vector<Key> &new_keys)
{
	for (auto &key : new_keys)
		keys[key] = false;
}

void SceneMoveAction::deactivated(ActivationChangeEvent &e)
{
	timer->stop();
	for (auto &it : keys)
		it.second = false;
	end_action();
}

void SceneMoveAction::focus_lost(FocusChangeEvent &e)
{
	timer->stop();
	for (auto &it : keys)
		it.second = false;
	end_action();
}

void SceneMoveAction::key_press(KeyEvent &e)
{
	auto it = keys.find(e.key());
	if (it != keys.end())
	{
		keys[e.key()] = true;
		timer->start(16);

		if (!action_active())
		{
			begin_action();
			gametime.update(); // Should be a reset, but this bugs for some reason
		}
	}
}

void SceneMoveAction::key_release(KeyEvent &e)
{
	auto it = keys.find(e.key());
	if (it != keys.end())
	{
		keys[e.key()] = false;

		bool stop = true;
		for (auto &it : keys)
			if (it.second)
				stop = false;

		if (stop)
		{
			timer->stop();
			end_action();
		}
	}
}
