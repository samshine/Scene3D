
#pragma once

#include "Scene3D/scene_engine.h"
#include "Scene3D/scene.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_light_probe.h"
#include "Scene3D/scene_model.h"
#include "Scene3D/scene_object.h"
#include "Scene3D/scene_particle_emitter.h"
#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/MapData/map_data.h"
#include "Scene3D/LevelData/level_data.h"
#include "Scene3D/Performance/gpu_timer.h"
#include "Scene3D/Performance/scope_timer.h"

#if defined(_MSC_VER)
	#if !defined(_DEBUG)
		#pragma comment(lib, "Scene3D.lib")
	#else
		#pragma comment(lib, "Scene3Dd.lib")
	#endif
#endif
