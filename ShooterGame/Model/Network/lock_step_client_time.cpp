
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
	return game_time.get_ticks_elapsed() + extra_ticks;
}

float LockStepClientTime::get_tick_time_elapsed() const
{
	return game_time.get_tick_time_elapsed();
}

int LockStepClientTime::get_tick_time_elapsed_ms() const
{
	return game_time.get_tick_time_elapsed_ms();
}

float LockStepClientTime::get_tick_interpolation_time() const
{
	return game_time.get_tick_interpolation_time();
}

float LockStepClientTime::get_updates_per_second() const
{
	return game_time.get_updates_per_second();
}

void LockStepClientTime::update()
{
	// Move time based on last update
	client_tick_time += get_ticks_elapsed();

	// Update time elapsed according to local time
	game_time.update();

	// Fetch how many ticks elapsed since last
	int ticks = game_time.get_ticks_elapsed();

	// Client tick time if it moved according to wall clock
	int new_client_tick_time = client_tick_time + ticks;

	// Continously move server time as if it was a wall clock
	server_tick_time += ticks;

	// Find how many ticks jitter can affect packet arrival:
	int jitter_ticks = (jitter + game_time.get_tick_time_elapsed_ms() - 1) / game_time.get_tick_time_elapsed_ms() + 2;

	// Calculate how many ticks pass before a packet reaches server:
	int ping_ticks = (ping + game_time.get_tick_time_elapsed_ms() - 1) / game_time.get_tick_time_elapsed_ms();

	// Find ideal client tick time
	int ideal_client_tick_time = server_tick_time - jitter_ticks;

	// Check how closely our client tick time matches the ideal client tick time:
	int sync_tick_delta = new_client_tick_time - ideal_client_tick_time;
	int sync_ms_delta = sync_tick_delta * game_time.get_tick_time_elapsed_ms();

	// Snap local time to server time if too much out of sync
	if (std::abs(sync_ms_delta) > 300)
	{
		//new_client_tick_time = ideal_client_tick_time;
		client_tick_time = server_tick_time - 5;
		Console::write_line("Client snapped to server time");
	}
	/*
	// Adjust ticks based on how our time is relative to the server
	if (new_client_tick_time < ideal_client_tick_time - 1)
	{
		// We are slightly behind of where we should be. Tick faster.
		extra_ticks = std::min(ideal_client_tick_time - new_client_tick_time, ticks);
	}
	else if (new_client_tick_time > ideal_client_tick_time + 1)
	{
		// We are slightly ahead of where we should be. Tick slower.
		extra_ticks = -std::min((new_client_tick_time % 1 + ticks) / 2, ticks);
	}
	else
	{
		extra_ticks = 0;
	}
	*/

	// Calculate what time sent messages should arrive on the server
	server_arrival_tick_time = client_tick_time + ping_ticks + jitter_ticks;

	// Send a ping packet once a second:
	next_send_ping = std::max(next_send_ping - (ticks - extra_ticks), 0);
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
	extra_ticks = 0;
	next_send_ping = 0;
}

void LockStepClientTime::on_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	if (net_event.get_name() == "LockStepPong")
	{
		unsigned int send_time = net_event.get_argument(0);
		server_tick_time = net_event.get_argument(1);

		actual_ping = (unsigned int)(System::get_time() - send_time);
		ping = 110;
		jitter = 20;
	}
}
