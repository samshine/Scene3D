
#pragma once

#include <memory>

class Physics3DShape;
class Physics3DObject;
typedef std::shared_ptr<Physics3DShape> Physics3DShapePtr;

class Physics3DTestResult
{
public:
	operator bool() const { return hit(); }
	bool operator<(const Physics3DTestResult &other) const { return fraction < other.fraction; }

	bool hit() const { return fraction != 1.0f; }

	float fraction = 1.0f;
	uicore::Vec3f position;
	uicore::Vec3f normal;
	Physics3DObject *object = nullptr;
};

class Physics3DWorld
{
public:
	static std::shared_ptr<Physics3DWorld> create();

	virtual void set_gravity(const uicore::Vec3f &gravity) = 0;

	virtual int step_simulation(float time_step, int max_sub_steps, float fixed_time_step) = 0;
	virtual void step_simulation_once(float time_step) = 0;

	virtual void ray_test(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<float(const Physics3DTestResult &result)> &hit_callback) = 0;
	virtual void sweep_test(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration, const std::function<float(const Physics3DTestResult &result)> &hit_callback) = 0;

	Physics3DTestResult ray_test_nearest(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());
	Physics3DTestResult ray_test_any(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());
	std::vector<Physics3DTestResult> ray_test_all(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());
	std::vector<Physics3DTestResult> ray_test_all_sorted(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());

	Physics3DTestResult sweep_test_nearest(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());
	Physics3DTestResult sweep_test_any(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());
	std::vector<Physics3DTestResult> sweep_test_all(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());
	std::vector<Physics3DTestResult> sweep_test_all_sorted(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f, const std::function<bool(const Physics3DTestResult &result)> &hit_predicate = std::function<bool(const Physics3DTestResult &)>());
};

typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
