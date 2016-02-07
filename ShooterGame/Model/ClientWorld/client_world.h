
#pragma once

#include <string>
#include <map>
#include "Model/Audio/music_player.h"
#include "Model/Input/input_buttons.h"

class ClientWorld
{
public:
	ClientWorld(const uicore::DisplayWindowPtr &window, const SceneEnginePtr &scene_engine, const std::shared_ptr<SoundCache> &sound_cache);
	~ClientWorld();

	static ClientWorld *current();

	void update(uicore::Vec2i mouse_movement, bool has_focus);

	uicore::DisplayWindowPtr window;

	SceneEnginePtr scene_engine;
	SceneCameraPtr scene_camera;
	ScenePtr scene;
	std::vector<SceneObjectPtr> objects;
	std::vector<SceneDecalPtr> decals;

	std::map<uicore::Vec3f, std::shared_ptr<SceneModel>> box_models;
	std::map<uicore::Vec2f, std::shared_ptr<SceneModel>> capsule_models;

	AudioWorldPtr audio;
	MusicPlayer music_player;

	InputButtons buttons;
	uicore::Point mouse_movement;
};

class ClientObject
{
public:
	ClientObject();
	~ClientObject();

	virtual void frame(float time_elapsed, float interpolated_time) { }

	ClientWorld *client_world();
};

typedef std::shared_ptr<ClientObject> ClientObjectPtr;
