
#pragma once

#include "lock_step_time.h"
#include "game_time.h"
#include "NetGame/event.h"
#include <memory>

class GameNetwork;

class LockStepServerTime : public LockStepTime
{
public:
	LockStepServerTime(std::shared_ptr<GameNetwork> network);
	~LockStepServerTime();

	int ticks_elapsed() const override;
	float tick_time_elapsed() const override;
	int tick_time_elapsed_ms() const override;
	float tick_interpolation_time() const override;
	float frame_time_elapsed() const override;

	int receive_tick_time() const override { return server_tick_time; }
	int arrival_tick_time() const override { return server_tick_time; }

	void update() override;
	void reset() override;

private:
	void on_event_received(const std::string &sender, const uicore::NetGameEvent &net_event);

	GameTime game_time;
	std::shared_ptr<GameNetwork> network;

	int server_tick_time = 0;

	uicore::SlotContainer slots;
};
