
#pragma once

#include "../Scene3D/Performance/gpu_timer.h"

class FrameStatistics
{
public:
	int frame_ms = 0;
	int models_drawn = 0;
	int instances_drawn = 0;
	int draw_calls = 0;
	int triangles_drawn = 0;
	int scene_visits = 0;
	int model_instance_maps = 0;
	std::vector<GPUTimer::Result> gpu_results;
	std::vector<std::string> cpu_results;
};

class ScreenController;
class SoundOutput;
class SoundCache;
class SceneEngine;
class SceneViewport;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;
typedef std::shared_ptr<SceneViewport> SceneViewportPtr;
class GameTime;

class Screen
{
public:
	static Screen *instance();

	virtual void run(std::function<std::shared_ptr<ScreenController>()> create_screen_controller, const std::string &title, const std::string &icon = std::string(), const std::string &cursor = std::string()) = 0;

	virtual const uicore::DisplayWindowPtr &window() = 0;
	virtual uicore::GraphicContextPtr &gc() = 0;
	virtual uicore::CanvasPtr &canvas() = 0;

	virtual SoundOutput &sound_output() = 0;
	virtual std::shared_ptr<SoundCache> &sound_cache() = 0;

	virtual SceneEnginePtr &scene_engine() = 0;
	virtual SceneViewportPtr &scene_viewport() = 0;

	virtual std::shared_ptr<ScreenController> &screen_controller() = 0;

	virtual GameTime &game_time() = 0;
	virtual uicore::Vec2i &delta_mouse_move() = 0;

	virtual const FrameStatistics &last_frame_stats() = 0;
};
