
#include "precomp.h"

using namespace clan;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DTarget::set_current();
	ResourceManager resources;
	//DisplayCache::set(resources, std::make_shared<SimpleDisplayCache>());

	//RunLoop::run();

	return 0;
}

