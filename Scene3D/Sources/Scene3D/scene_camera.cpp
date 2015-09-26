
#include "precomp.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/scene.h"
#include "scene_camera_impl.h"
#include "scene_impl.h"

using namespace uicore;

SceneCamera::SceneCamera()
{
}

SceneCamera::SceneCamera(Scene &scene)
: impl(std::make_shared<SceneCamera_Impl>())
{
	impl->scene = scene.impl.get();
}

bool SceneCamera::is_null() const
{
	return !impl;
}

Vec3f SceneCamera::get_position() const
{
	return impl->position;
}

Quaternionf SceneCamera::get_orientation() const
{
	return impl->orientation;
}

float SceneCamera::get_field_of_view() const
{
	return impl->fov;
}

void SceneCamera::set_position(const Vec3f &position)
{
	impl->position = position;
}

void SceneCamera::set_orientation(const Quaternionf &orientation)
{
	impl->orientation = orientation;
}

void SceneCamera::set_field_of_view(float fov)
{
	impl->fov = fov;
}
