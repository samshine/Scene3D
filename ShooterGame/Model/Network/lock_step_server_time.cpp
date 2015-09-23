
#include "precomp.h"
#include "lock_step_server_time.h"
#include "game_network.h"

using namespace uicore;

LockStepServerTime::LockStepServerTime(std::shared_ptr<GameNetwork> network)
	: game_time(ticks_per_second), network(network)
{
	slots.connect(network->sig_event_received, this, &LockStepServerTime::on_event_received);
}

LockStepServerTime::~LockStepServerTime()
{
}

int LockStepServerTime::get_ticks_elapsed() const
{
	return game_time.get_ticks_elapsed();
}

float LockStepServerTime::get_tick_time_elapsed() const
{
	return game_time.get_tick_time_elapsed();
}

int LockStepServerTime::get_tick_time_elapsed_ms() const
{
	return game_time.get_tick_time_elapsed_ms();
}

float LockStepServerTime::get_tick_interpolation_time() const
{
	return game_time.get_tick_interpolation_time();
}

float LockStepServerTime::get_updates_per_second() const
{
	return game_time.get_updates_per_second();
}

void LockStepServerTime::update()
{
	server_tick_time += get_ticks_elapsed();
	game_time.update();
}

void LockStepServerTime::reset()
{
	game_time.reset();
	server_tick_time = 0;
}

void LockStepServerTime::on_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	if (net_event.get_name() == "LockStepPing")
	{
		NetGameEvent pong("LockStepPong");
		for (unsigned int i = 0; i < net_event.get_argument_count(); i++)
			pong.add_argument(net_event.get_argument(i));

		pong.add_argument(server_tick_time);

		network->queue_event(sender, pong);
	}
}
