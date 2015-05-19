
#pragma once

#include "lock_step_time.h"
#include <memory>

class GameNetwork;

class LockStepClientTime : public LockStepTime
{
public:
	LockStepClientTime(std::shared_ptr<GameNetwork> network);
	~LockStepClientTime();

	int get_ticks_elapsed() const override;
	float get_tick_time_elapsed() const override;
	int get_tick_time_elapsed_ms() const override;
	float get_tick_interpolation_time() const override;
	float get_updates_per_second() const override;

	int get_receive_tick_time() const override { return client_tick_time; }
	int get_arrival_tick_time() const override { return server_arrival_tick_time; }

	void update() override;
	void reset() override;

	static int actual_ping;

private:
	void on_event_received(const std::string &sender, const clan::NetGameEvent &net_event);

	clan::GameTime game_time;
	std::shared_ptr<GameNetwork> network;

	int extra_ticks = 0;
	int server_arrival_tick_time = 0;

	int server_tick_time = 0;
	int client_tick_time = 0;

	int ping = 0;
	int jitter = 0;

	int next_send_ping = 0;

	clan::SlotContainer slots;
};
