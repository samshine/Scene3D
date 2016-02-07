
#pragma once

class GameTick
{
public:
	GameTick() { }
	GameTick(float time_elapsed, int receive_tick_time, int arrival_tick_time) : time_elapsed(time_elapsed), receive_tick_time(receive_tick_time), arrival_tick_time(arrival_tick_time) { }

	float time_elapsed = 0.0f;
	int receive_tick_time = 0;
	int arrival_tick_time = 0;
};
