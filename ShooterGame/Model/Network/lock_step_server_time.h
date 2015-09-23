
#pragma once

#include "lock_step_time.h"
#include "NetGame/event.h"
#include <memory>

class GameNetwork;

class LockStepServerTime : public LockStepTime
{
public:
	LockStepServerTime(std::shared_ptr<GameNetwork> network);
	~LockStepServerTime();

	int get_ticks_elapsed() const override;
	float get_tick_time_elapsed() const override;
	int get_tick_time_elapsed_ms() const override;
	float get_tick_interpolation_time() const override;
	float get_updates_per_second() const override;

	int get_receive_tick_time() const override { return server_tick_time; }
	int get_arrival_tick_time() const override { return server_tick_time; }

	void update() override;
	void reset() override;

private:
	void on_event_received(const std::string &sender, const uicore::NetGameEvent &net_event);

	uicore::GameTime game_time;
	std::shared_ptr<GameNetwork> network;

	int server_tick_time = 0;

	uicore::SlotContainer slots;
};
