
#pragma once

#include <memory>

class GPUTimerImpl;

class GPUTimer
{
public:
	GPUTimer();

	void begin_frame(const uicore::GraphicContextPtr &gc);

	void begin_time(const uicore::GraphicContextPtr &gc, const std::string &name);
	void end_time(const uicore::GraphicContextPtr &gc);

	void end_frame(const uicore::GraphicContextPtr &gc);

	struct Result
	{
		Result(std::string name, float time_elapsed) : name(name), time_elapsed(time_elapsed) { }
		std::string name;
		float time_elapsed;
	};

	std::vector<Result> get_results(const uicore::GraphicContextPtr &gc);

private:
	std::shared_ptr<GPUTimerImpl> impl;
};
