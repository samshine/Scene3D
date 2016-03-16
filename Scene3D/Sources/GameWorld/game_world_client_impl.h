
#pragma once

#include "GameWorld/game_world_client.h"
#include "Scene3D/scene.h"
#include "Scene3D/scene_engine.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/scene_object.h"
#include "Scene3D/scene_decal.h"
#include "Sound/AudioWorld/audio_world.h"
#include "Sound/music_player.h"
#include "input_buttons.h"

class GameWorld;

class GameWorldClientImpl
{
public:
	GameWorldClientImpl(const uicore::DisplayWindowPtr &window, const SceneEnginePtr &scene_engine, const std::shared_ptr<SoundCache> &sound_cache);

	void update_input(GameWorld *game_world, uicore::Vec2i mouse_movement, bool has_focus);
	void update(GameWorld *game_world);

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

	friend class ClientObject;
};
