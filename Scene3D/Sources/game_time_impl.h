
#pragma once

class GameTime_Impl
{
public:
	GameTime_Impl(int ticks_per_second, int max_updates_per_second) : ticks_per_second(ticks_per_second)
	{
		if (max_updates_per_second)
			min_update_time_ms = (1000 / max_updates_per_second);
	}

	void update();
	void reset();

	int ticks_per_second = 0;
	int min_update_time_ms = 0;

	int64_t tick_time_adjustment = 0;

	int64_t start_time = 0;
	int64_t current_time = 0;
	int64_t last_tick_time = 0;

	float time_elapsed = 0.0f;
	int time_elapsed_ms = 0;
	int time_elapsed_ms_microsecond_adjustment = 0;		// Amount of Microseconds lost due to time_elapsed_ms rounding (to add on for next time)

	int ticks_elapsed = 0;
	float tick_interpolation_time = 0.0f;

	int64_t update_frame_start_time = 0;
	float current_fps = 0.0f;

private:
	void calculate_fps();
	void process_max_fps();

	int num_updates_in_2_seconds = 0;
	int update_time_ms = 0;
};
