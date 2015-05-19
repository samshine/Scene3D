
#pragma once

class LockStepTime
{
public:
	virtual ~LockStepTime() { }

	virtual int get_ticks_elapsed() const = 0;
	virtual float get_tick_time_elapsed() const = 0;
	virtual int get_tick_time_elapsed_ms() const = 0;
	virtual float get_tick_interpolation_time() const = 0;
	virtual float get_updates_per_second() const = 0;

	virtual int get_receive_tick_time() const = 0;
	virtual int get_arrival_tick_time() const = 0;

	virtual void update() = 0;
	virtual void reset() = 0;

	static const int ticks_per_second = 60;
};
