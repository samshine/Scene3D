
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

int LockStepServerTime::ticks_elapsed() const
{
	return game_time.ticks_elapsed();
}

float LockStepServerTime::tick_time_elapsed() const
{
	return game_time.tick_time_elapsed();
}

int LockStepServerTime::tick_time_elapsed_ms() const
{
	return game_time.tick_time_elapsed_ms();
}

float LockStepServerTime::tick_interpolation_time() const
{
	return game_time.tick_interpolation_time();
}

float LockStepServerTime::frame_time_elapsed() const
{
	return game_time.time_elapsed();
}

void LockStepServerTime::update()
{
	server_tick_time += ticks_elapsed();
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
