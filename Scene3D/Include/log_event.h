
#pragma once

class LogEvent
{
public:
	static uicore::Signal<void(const std::string &type, const std::string &text)> &sig_log_event();
};

inline void log_event(const std::string &type, const std::string &text)
{
	return LogEvent::sig_log_event()(type, text);
}

template <class... Args>
void log_event(const std::string &type, const std::string &format, Args && ... args)
{
	log_event(type, uicore::string_format(format, std::forward<Args>(args)...));
}
