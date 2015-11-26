
#pragma once

#include <memory>

class SceneEngine;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;

class Scene
{
public:
	static std::shared_ptr<Scene> create(const SceneEnginePtr &engine);

	virtual void set_cull_oct_tree(const uicore::AxisAlignedBoundingBox &aabb) = 0;
	virtual void set_cull_oct_tree(const uicore::Vec3f &aabb_min, const uicore::Vec3f &aabb_max) = 0;
	virtual void set_cull_oct_tree(float max_size) = 0;

	virtual void show_skybox_stars(bool enable) = 0;
	virtual void set_skybox_gradient(std::vector<uicore::Colorf> &colors) = 0;
};

typedef std::shared_ptr<Scene> ScenePtr;
