
#pragma once

#include "lock_step_time.h"
#include "NetGame/event.h"
#include <memory>

class GameNetwork;

class LockStepClientTime : public LockStepTime
{
public:
	LockStepClientTime(std::shared_ptr<GameNetwork> network);
	~LockStepClientTime();

	int ticks_elapsed() const override;
	float tick_time_elapsed() const override;
	int tick_time_elapsed_ms() const override;
	float tick_interpolation_time() const override;

	int receive_tick_time() const override;
	int arrival_tick_time() const override;

	void update() override;
	void reset() override;

	static int actual_ping;

private:
	void on_event_received(const std::string &sender, const uicore::NetGameEvent &net_event);

	uicore::GameTime game_time;
	std::shared_ptr<GameNetwork> network;

	int server_tick_time = 0;
	int client_tick_time = 0;
	int last_client_tick_time = 0;
	int jitter_ticks = 0;
	int ping_ticks = 0;

	int next_send_ping = 0;

	uicore::SlotContainer slots;
};
