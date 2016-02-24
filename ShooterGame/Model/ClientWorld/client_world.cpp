
#include "precomp.h"
#include "client_world.h"

using namespace uicore;

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

void ClientWorld::update(uicore::Vec2i new_mouse_movement, bool has_focus)
{
	if (has_focus)
	{
		mouse_movement = new_mouse_movement;
		buttons.update(window);
	}
	else
	{
		mouse_movement = Point();
		buttons.input_disabled();
	}

	float time_elapsed = GameWorld::current()->frame_time_elapsed();
	float interpolated_time = GameWorld::current()->frame_interpolated_time();

	for (auto &object : objects)
		object->update(time_elapsed);

	for (auto &obj : client_objects)
		obj->frame(time_elapsed, interpolated_time);

	music_player.update();

	audio->set_listener(scene_camera->position(), scene_camera->orientation());
	audio->update();
}

/////////////////////////////////////////////////////////////////////////////

ClientObject::ClientObject()
{
	auto &objects = ClientWorld::current()->client_objects;
	iterator = objects.insert(objects.end(), this);
}

ClientObject::~ClientObject()
{
	ClientWorld *world = ClientWorld::current();
	if (world)
		world->client_objects.erase(iterator);
}

ClientWorld *ClientObject::client_world()
{
	return ClientWorld::current();
}
