
#include "precomp.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/scene.h"
#include "scene_camera_impl.h"
#include "scene_impl.h"

using namespace uicore;

std::shared_ptr<SceneCamera> SceneCamera::create(const ScenePtr &scene)
{
	return std::make_shared<SceneCamera_Impl>(static_cast<Scene_Impl*>(scene.get()));
}
