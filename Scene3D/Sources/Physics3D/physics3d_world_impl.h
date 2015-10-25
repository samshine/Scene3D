
#pragma once

#include <memory>
#include "Physics3D/physics3d_world.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include "Physics3D/Bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "physics3d_object_impl.h"

class Physics3DWorldImpl : public Physics3DWorld
{
public:
	Physics3DWorldImpl();
	~Physics3DWorldImpl();

	void set_gravity(const uicore::Vec3f &gravity) override;
	int step_simulation(float time_step, int max_sub_steps, float fixed_time_step) override;
	void step_simulation_once(float time_step) override;

	void ray_test(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<float(const Physics3DTestResult &result)> &hit_callback) override;
	void sweep_test(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration, const std::function<float(const Physics3DTestResult &result)> &hit_callback) override;

	std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
	std::unique_ptr<btConstraintSolver> constraint_solver;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<btGhostPairCallback> ghost_pair_callback;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;
};

class Physics3DSweepResultCallback : public btCollisionWorld::ConvexResultCallback
{
public:
	Physics3DSweepResultCallback(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<float(const Physics3DTestResult &result)> &hit_callback) : start(start), end(end), hit_callback(hit_callback)
	{
	}

	btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace) override
	{
		Physics3DTestResult hit;
		hit.fraction = convexResult.m_hitFraction;
		hit.position = mix(start, end, hit.fraction);
		hit.object = static_cast<Physics3DObjectImpl*>(convexResult.m_hitCollisionObject->getUserPointer());

		if (normalInWorldSpace)
			hit.normal = to_vec3f(convexResult.m_hitNormalLocal);
		else
			hit.normal = to_vec3f(convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal);

		return btScalar(hit_callback(hit));
	}

private:
	static uicore::Vec3f to_vec3f(const btVector3 &v)
	{
		return uicore::Vec3f(v.getX(), v.getY(), v.getZ());
	}

	uicore::Vec3f start;
	uicore::Vec3f end;
	const std::function<float(const Physics3DTestResult &result)> &hit_callback;
};

class Physics3DRayResultCallback : public btCollisionWorld::RayResultCallback
{
public:
	Physics3DRayResultCallback(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<float(const Physics3DTestResult &result)> &hit_callback) : start(start), end(end), hit_callback(hit_callback)
	{
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace) override
	{
		Physics3DTestResult hit;
		hit.fraction = rayResult.m_hitFraction;
		hit.position = mix(start, end, hit.fraction);
		hit.object = static_cast<Physics3DObjectImpl*>(rayResult.m_collisionObject->getUserPointer());

		if (normalInWorldSpace)
			hit.normal = to_vec3f(rayResult.m_hitNormalLocal);
		else
			hit.normal = to_vec3f(rayResult.m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal);

		return btScalar(hit_callback(hit));
	}

private:
	static uicore::Vec3f to_vec3f(const btVector3 &v)
	{
		return uicore::Vec3f(v.getX(), v.getY(), v.getZ());
	}

	uicore::Vec3f start;
	uicore::Vec3f end;
	const std::function<float(const Physics3DTestResult &result)> &hit_callback;
};
