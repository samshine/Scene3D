
#include "precomp.h"
#include "Scene3D/scene_engine.h"
#include "scene_engine_impl.h"

using namespace uicore;

std::shared_ptr<SceneEngine> SceneEngine::create()
{
	return std::make_shared<SceneEngineImpl>();
}
