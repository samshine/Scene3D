
#include "precomp.h"
#include "log_event.h"

using namespace uicore;

Signal<void(const std::string &type, const std::string &text)> &LogEvent::sig_log_event()
{
	static Signal<void(const std::string &type, const std::string &text)> signal;
	return signal;
}
