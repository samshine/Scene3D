
#pragma once

#include <string>
#include <map>
#include <list>

class GameWorldClientImpl;
class SceneEngine;
class SceneObject;
class SceneDecal;
class SceneModel;
class SceneCamera;
class Scene;
class SoundCache;
class AudioWorld;
class MusicPlayer;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;
typedef std::shared_ptr<SceneCamera> SceneCameraPtr;
typedef std::shared_ptr<SceneObject> SceneObjectPtr;
typedef std::shared_ptr<SceneDecal> SceneDecalPtr;
typedef std::shared_ptr<Scene> ScenePtr;
typedef std::shared_ptr<AudioWorld> AudioWorldPtr;

class GameWorldClient
{
public:
	GameWorldClient(const uicore::DisplayWindowPtr &window, const SceneEnginePtr &scene_engine, const std::shared_ptr<SoundCache> &sound_cache);
	~GameWorldClient();

	uicore::DisplayWindowPtr &window();

	SceneEnginePtr &scene_engine();
	SceneCameraPtr &scene_camera();
	ScenePtr &scene();
	std::vector<SceneObjectPtr> &objects();
	std::vector<SceneDecalPtr> &decals();

	std::map<uicore::Vec3f, std::shared_ptr<SceneModel>> &box_models();
	std::map<uicore::Vec2f, std::shared_ptr<SceneModel>> &capsule_models();

	AudioWorldPtr &audio();
	MusicPlayer &music_player();

	bool button_down(const std::string &name);
	uicore::Point mouse_movement();

	void load_buttons(const std::string &filename);

private:
	GameWorldClient(const GameWorldClient &) = delete;
	GameWorldClient &operator=(const GameWorldClient &) = delete;

	std::unique_ptr<GameWorldClientImpl> impl;

	friend class GameWorld;
	friend class GameWorldImpl;
};
