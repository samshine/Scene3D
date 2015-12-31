
#include "precomp.h"
#include "lock_step_client_time.h"
#include "game_network.h"
#include <algorithm>

using namespace uicore;

int LockStepClientTime::actual_ping = 0;

LockStepClientTime::LockStepClientTime(std::shared_ptr<GameNetwork> network)
	: game_time(ticks_per_second), network(network)
{
	slots.connect(network->sig_event_received, this, &LockStepClientTime::on_event_received);
}

LockStepClientTime::~LockStepClientTime()
{
}

int LockStepClientTime::get_ticks_elapsed() const
{
	return client_tick_time - last_client_tick_time;
}

float LockStepClientTime::get_tick_time_elapsed() const
{
	return game_time.tick_time_elapsed();
}

int LockStepClientTime::get_tick_time_elapsed_ms() const
{
	return game_time.tick_time_elapsed_ms();
}

float LockStepClientTime::get_tick_interpolation_time() const
{
	return game_time.tick_interpolation_time();
}

float LockStepClientTime::get_updates_per_second() const
{
	return game_time.updates_per_second();
}

int LockStepClientTime::get_receive_tick_time() const
{
	return last_client_tick_time;
}

int LockStepClientTime::get_arrival_tick_time() const
{
	return client_tick_time + ping_ticks + jitter_ticks;
}

void LockStepClientTime::update()
{
	game_time.update();

	last_client_tick_time = client_tick_time;

	for (int i = 0; i < game_time.ticks_elapsed(); i++)
	{
		client_tick_time++;
		server_tick_time++;

		int tick_delta = client_tick_time - server_tick_time;

		if (tick_delta > 0 || std::abs(tick_delta) > jitter_ticks * 2) // Warp time if client is ahead of server or too far out of sync
		{
			client_tick_time = server_tick_time - jitter_ticks;
			last_client_tick_time = client_tick_time;
			log_event("net", "Re-syncing. Ping: %1 ms, Tick delta: %2, jitter: %3, ping ticks: %4", actual_ping, tick_delta, jitter_ticks, ping_ticks);
			break;
		}
		else if (tick_delta > -jitter_ticks) // Slow time if client is too close to server
		{
			game_time.set_tick_time_adjustment(5000);
		}
		else if (tick_delta < -jitter_ticks) // Speed up time if client is too far away from server
		{
			game_time.set_tick_time_adjustment(-5000);
		}
		else
		{
			game_time.set_tick_time_adjustment(0);
		}
	}

	// Send a ping packet once a second:
	next_send_ping = std::max(next_send_ping - game_time.ticks_elapsed(), 0);
	if (next_send_ping == 0)
	{
		network->queue_event("server", NetGameEvent("LockStepPing", { (unsigned int)System::get_time() }));
		next_send_ping = ticks_per_second;
	}
}

void LockStepClientTime::reset()
{
	game_time.reset();
	client_tick_time = 0;
	server_tick_time = 0;
	next_send_ping = 0;
	ping_ticks = 0;
	jitter_ticks = 0;
}

void LockStepClientTime::on_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	if (net_event.get_name() == "LockStepPong")
	{
		unsigned int send_time = net_event.get_argument(0);
		server_tick_time = net_event.get_argument(1);

		actual_ping = (unsigned int)(System::get_time() - send_time);

		ping_ticks = 6; // 96 ms ping
		jitter_ticks = 2; // 32 ms jitter
	}
}
