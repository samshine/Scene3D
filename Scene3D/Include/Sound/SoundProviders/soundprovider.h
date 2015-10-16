
#pragma once

#include <memory>

class SoundProvider_Session;
class SoundProviderImpl;

class SoundProvider
{
public:
	SoundProvider();
	virtual ~SoundProvider();

	virtual SoundProvider_Session *begin_session() = 0;
	virtual void end_session(SoundProvider_Session *session) = 0;

private:
	std::shared_ptr<SoundProviderImpl> impl;
};
