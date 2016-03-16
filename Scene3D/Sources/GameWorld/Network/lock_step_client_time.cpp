
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

int LockStepClientTime::ticks_elapsed() const
{
#ifdef _DEBUG
	return std::min(client_tick_time - last_client_tick_time, 3);
#else
	return client_tick_time - last_client_tick_time;
#endif
}

float LockStepClientTime::tick_time_elapsed() const
{
	return game_time.tick_time_elapsed();
}

int LockStepClientTime::tick_time_elapsed_ms() const
{
	return game_time.tick_time_elapsed_ms();
}

float LockStepClientTime::tick_interpolation_time() const
{
	return game_time.tick_interpolation_time();
}

float LockStepClientTime::frame_time_elapsed() const
{
	return game_time.time_elapsed();
}

int LockStepClientTime::receive_tick_time() const
{
	return last_client_tick_time;
}

int LockStepClientTime::arrival_tick_time() const
{
	return client_tick_time + ping_ticks + jitter_ticks;
}

void LockStepClientTime::update()
{
	game_time.update();

	/*static FilePtr file = File::create_always("c:\\development\\debug.csv");
	static uint64_t start_time = System::microseconds();
	auto str = string_format("%1;%2;%3\r\n", (System::microseconds() - start_time) / 1000.0, game_time.ticks_elapsed(), game_time.tick_interpolation_time());
	file->write(str.data(), str.length());*/

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
			//log_event("net", "Re-syncing. Ping: %1 ms, Tick delta: %2, jitter: %3, ping ticks: %4", actual_ping, tick_delta, jitter_ticks, ping_ticks);
			break;
		}
		else if (tick_delta > -jitter_ticks) // Slow time if client is too close to server
		{
			game_time.set_tick_time_adjustment(2500);
		}
		else if (tick_delta < -jitter_ticks) // Speed up time if client is too far away from server
		{
			game_time.set_tick_time_adjustment(-2500);
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
		network->queue_event("server", NetGameEvent("LockStepPing", { (unsigned int)System::time() }));
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

		actual_ping = (unsigned int)(System::time() - send_time);

		ping_ticks = 4; // 128 ms ping means it roughly arrives at server after 64 ms (each tick is 16 ms, 4 * 16 * 2 = 128)
		jitter_ticks = 2; // 32 ms jitter
	}
}
