
#pragma once

#include <memory>

class GPUTimer_Impl;

class GPUTimer
{
public:
	GPUTimer();

	void begin_frame(uicore::GraphicContext &gc);

	void begin_time(uicore::GraphicContext &gc, const std::string &name);
	void end_time(uicore::GraphicContext &gc);

	void end_frame(uicore::GraphicContext &gc);

	struct Result
	{
		Result(std::string name, float time_elapsed) : name(name), time_elapsed(time_elapsed) { }
		std::string name;
		float time_elapsed;
	};

	std::vector<Result> get_results(uicore::GraphicContext &gc);

private:
	std::shared_ptr<GPUTimer_Impl> impl;
};
