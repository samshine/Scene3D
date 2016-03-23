
#pragma once

#include "Performance/gpu_timer.h"
#include <memory>
#include <string>

class SceneEngine
{
public:
	static std::shared_ptr<SceneEngine> create();

	virtual void wait_next_frame_ready(const uicore::GraphicContextPtr &gc) = 0;

	virtual int models_drawn() const = 0;
	virtual int instances_drawn() const = 0;
	virtual int draw_calls() const = 0;
	virtual int triangles_drawn() const = 0;
	virtual int scene_visits() const = 0;
	virtual int model_instance_maps() const = 0;
	virtual const std::vector<GPUTimer::Result> &gpu_results() const = 0;
};

typedef std::shared_ptr<SceneEngine> SceneEnginePtr;
