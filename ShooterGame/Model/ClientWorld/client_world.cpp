
#include "precomp.h"
#include "client_world.h"

namespace
{
#if _MSC_VER < 1900
	ClientWorld __declspec(thread) *instance = nullptr;
#else
	thread_local ClientWorld *instance = nullptr;
#endif
}

ClientWorld::ClientWorld(const uicore::DisplayWindowPtr &window, const SceneEnginePtr &scene_engine, const std::shared_ptr<SoundCache> &sound_cache) : window(window), scene_engine(scene_engine)
{
	audio = AudioWorld::create(sound_cache);
	scene = Scene::create(scene_engine);
	scene_camera = SceneCamera::create(scene);

	instance = this;
}

ClientWorld::~ClientWorld()
{
	instance = nullptr;
}

ClientWorld *ClientWorld::current()
{
	return instance;
}

/////////////////////////////////////////////////////////////////////////////

ClientObject::ClientObject()
{
}

ClientObject::~ClientObject()
{
}

ClientWorld *ClientObject::client_world()
{
	return ClientWorld::current();
}
