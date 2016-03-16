
#include "precomp.h"
#include "GameWorld/game_world_client.h"
#include "GameWorld/game_world.h"
#include "game_world_client_impl.h"

using namespace uicore;

GameWorldClient::GameWorldClient(const uicore::DisplayWindowPtr &window, const SceneEnginePtr &scene_engine, const std::shared_ptr<SoundCache> &sound_cache)
	: impl(new GameWorldClientImpl(window, scene_engine, sound_cache))
{
}

GameWorldClient::~GameWorldClient()
{
}

DisplayWindowPtr &GameWorldClient::window()
{
	return impl->window;
}

SceneEnginePtr &GameWorldClient::scene_engine()
{
	return impl->scene_engine;
}

SceneCameraPtr &GameWorldClient::scene_camera()
{
	return impl->scene_camera;
}

ScenePtr &GameWorldClient::scene()
{
	return impl->scene;
}

std::vector<SceneObjectPtr> &GameWorldClient::objects()
{
	return impl->objects;
}

std::vector<SceneDecalPtr> &GameWorldClient::decals()
{
	return impl->decals;
}

std::map<Vec3f, std::shared_ptr<SceneModel>> &GameWorldClient::box_models()
{
	return impl->box_models;
}

std::map<Vec2f, std::shared_ptr<SceneModel>> &GameWorldClient::capsule_models()
{
	return impl->capsule_models;
}

AudioWorldPtr &GameWorldClient::audio()
{
	return impl->audio;
}

MusicPlayer &GameWorldClient::music_player()
{
	return impl->music_player;
}

bool GameWorldClient::button_down(const std::string &name)
{
	return impl->buttons.buttons[name].down;
}

Point GameWorldClient::mouse_movement()
{
	return impl->mouse_movement;
}

void GameWorldClient::load_buttons(const std::string &filename)
{
	auto json = JsonValue::parse(File::read_all_text(filename));
	impl->buttons.load(window(), json["buttons"]);
}

/////////////////////////////////////////////////////////////////////////////

GameWorldClientImpl::GameWorldClientImpl(const uicore::DisplayWindowPtr &window, const SceneEnginePtr &scene_engine, const std::shared_ptr<SoundCache> &sound_cache) : window(window), scene_engine(scene_engine)
{
	audio = AudioWorld::create(sound_cache);
	scene = Scene::create(scene_engine);
	scene_camera = SceneCamera::create(scene);
}

void GameWorldClientImpl::update_input(GameWorld *game_world, uicore::Vec2i new_mouse_movement, bool has_focus)
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
}

void GameWorldClientImpl::update(GameWorld *game_world)
{
	float time_elapsed = game_world->frame_time_elapsed();
	for (auto &object : objects)
		object->update(time_elapsed);

	music_player.update();

	audio->set_listener(scene_camera->position(), scene_camera->orientation());
	audio->update();
}
