
#include "precomp.h"
#include "Scene3D/Performance/scope_timer.h"

using namespace uicore;

namespace
{
#if _MSC_VER < 1900
	ScopeTimerResults __declspec(thread) *instance;
#else
	thread_local ScopeTimerResults *instance;
#endif
}

ScopeTimerResults::ScopeTimerResults()
{
	if (instance == nullptr)
		instance = this;
}

ScopeTimerResults::~ScopeTimerResults()
{
	if (instance == this)
		instance = nullptr;
}

void ScopeTimerResults::start()
{
	if (instance == nullptr)
		return;

	instance->current_results.clear();
	instance->current_start_time = rdtsc();
	instance->current_end_time = instance->current_start_time;
	instance->current_start_microseconds = System::microseconds();
}

void ScopeTimerResults::add_result(const ScopeTimerResult &result)
{
	if (instance == nullptr)
		return;

	for (size_t i = 0; i < instance->current_results.size(); i++)
	{
		if (instance->current_results[i].name == result.name)
		{
			instance->current_results[i].ticks += result.ticks;
			return;
		}
	}
	instance->current_results.push_back(result);
}

void ScopeTimerResults::end()
{
	if (instance == nullptr)
		return;

	instance->current_end_time = rdtsc();
	instance->results = instance->current_results;
	instance->start_time = instance->current_start_time;
	instance->end_time = instance->current_end_time;
	instance->elapsed_microseconds = System::microseconds() - instance->current_start_microseconds;
}

int ScopeTimerResults::percentage(const char *name)
{
	if (instance == nullptr)
		return 0;

	uint64_t elapsed = instance->end_time - instance->start_time;
	if (elapsed != 0)
	{
		uint64_t ticks = 0;
		for (size_t i = 0; i < instance->results.size(); i++)
		{
			if (instance->results[i].name == name)
				ticks += instance->results[i].ticks;
		}
		return (int)(ticks * 100 / elapsed);
	}
	else
	{
		return 0;
	}
}

float ScopeTimerResults::milliseconds(const char *name)
{
	if (instance == nullptr)
		return 0.0f;

	uint64_t elapsed = instance->end_time - instance->start_time;
	if (elapsed != 0)
	{
		uint64_t ticks = 0;
		for (size_t i = 0; i < instance->results.size(); i++)
		{
			if (instance->results[i].name == name)
				ticks += instance->results[i].ticks;
		}
		return (ticks * instance->elapsed_microseconds / elapsed) / 1000.0f;
	}
	else
	{
		return 0.0f;
	}
}

std::vector<std::string> ScopeTimerResults::timer_results()
{
	if (instance == nullptr)
		return std::vector<std::string>();

	std::map<const char *, bool> result_names;
	for (size_t i = 0; i < instance->results.size(); i++)
		result_names[instance->results[i].name] = true;

	std::vector<std::pair<const char *, float>> summed_results;
	for (auto it : result_names)
	{
		float time_spent = milliseconds(it.first);
		if (time_spent >= 1.0f)
			summed_results.push_back({ it.first, time_spent });
	}

	std::sort(summed_results.begin(), summed_results.end(), [](const std::pair<const char *, float> &a, const std::pair<const char *, float> &b) { return a.second > b.second; });

	std::vector<std::string> final_results;
	for (const auto &result : summed_results)
	{
		std::string name = result.first;
		while (true)
		{
			auto pos = name.find("::");
			if (pos == std::string::npos)
				break;
			name.replace(pos, 2, ".");
		}
		final_results.push_back(string_format("%1: %2 ms", name, Text::to_string(result.second, 2, false)));
	}
	return final_results;
}
