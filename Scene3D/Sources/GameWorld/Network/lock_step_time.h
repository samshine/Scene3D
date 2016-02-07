
#pragma once

class LockStepTime
{
public:
	virtual ~LockStepTime() { }

	virtual int ticks_elapsed() const = 0;
	virtual float tick_time_elapsed() const = 0;
	virtual int tick_time_elapsed_ms() const = 0;
	virtual float tick_interpolation_time() const = 0;

	virtual float frame_time_elapsed() const = 0;

	virtual int receive_tick_time() const = 0;
	virtual int arrival_tick_time() const = 0;

	virtual void update() = 0;
	virtual void reset() = 0;

	static const int ticks_per_second = 60;
};
