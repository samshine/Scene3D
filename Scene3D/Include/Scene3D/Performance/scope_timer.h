
#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#if defined(_MSC_VER)

static inline unsigned long long rdtsc()
{
	return __rdtsc();
}

#elif defined(__i386__)

static __inline__ unsigned long long rdtsc()
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ unsigned long long rdtsc()
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#else

static __inline__ unsigned long long rdtsc()
{
	return 0;
}

#endif

class ScopeTimerResult
{
public:
	ScopeTimerResult(const char *name, unsigned long long ticks) : name(name), ticks(ticks) { }

	const char *name;
	unsigned long long ticks;
};

class ScopeTimerResults
{
public:
	static void start();
	static void add_result(const ScopeTimerResult &result);
	static void end();
	static int percentage(const char *name);
	static std::string timer_results();

private:
	ScopeTimerResults();

	unsigned long long start_time;
	std::vector<ScopeTimerResult> results;
	unsigned long long end_time;

	unsigned long long current_start_time;
	std::vector<ScopeTimerResult> current_results;
	unsigned long long current_end_time;

	static ScopeTimerResults instance;
};

class ScopeTimer
{
public:
	ScopeTimer() : _name(), _start()
	{
	}

	ScopeTimer(const char *name) : _name(), _start()
	{
		start(name);
	}

	~ScopeTimer()
	{
		end();
	}

	void start(const char *name)
	{
		end();
		_name = name;
		_start = rdtsc();
	}

	void end()
	{
		if (_name)
		{
			ScopeTimerResults::add_result(ScopeTimerResult(_name, rdtsc() - _start));
			_name = 0;
		}
	}

private:
	ScopeTimer(const ScopeTimer &that);
	ScopeTimer &operator =(const ScopeTimer &that);

	const char *_name;
	unsigned long long _start;
};

#define ScopeTimeFunction() ScopeTimer scope_timer(__FUNCTION__);
