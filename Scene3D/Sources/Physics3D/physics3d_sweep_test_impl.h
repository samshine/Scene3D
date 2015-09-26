
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <vector>

class Physics3DObject_Impl;
class Physics3DWorld_Impl;

class Physics3DSweepTest_Impl
{
public:
	Physics3DSweepTest_Impl(Physics3DWorld_Impl *world);
	~Physics3DSweepTest_Impl();

	Physics3DWorld_Impl *world;

	uicore::Vec3f from_pos, to_pos;

	struct SweepHit
	{
		SweepHit() : hit_fraction(0.0f), hit_collision_object(0) { }

		bool operator<(const SweepHit &other) const { return hit_fraction < other.hit_fraction; }

		float hit_fraction;
		uicore::Vec3f hit_normal;
		Physics3DObject_Impl *hit_collision_object;
	};

	std::vector<SweepHit> hits;

	class AnyHitConvexResultCallback : public btCollisionWorld::ConvexResultCallback
	{
	public:
		AnyHitConvexResultCallback(const btVector3 &convexFromWorld, const btVector3 &convexToWorld)
			: m_convexFromWorld(convexFromWorld), m_hitFraction(btScalar(1.)), m_convexToWorld(convexToWorld), m_hitCollisionObject(0)
		{
		}

		btVector3 m_convexFromWorld;
		btVector3 m_convexToWorld;
		btScalar m_hitFraction;
		btVector3 m_hitNormalWorld;
		btVector3 m_hitPointWorld;
		const btCollisionObject *m_hitCollisionObject;

		virtual	btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			m_hitFraction = convexResult.m_hitFraction;
			m_hitCollisionObject = convexResult.m_hitCollisionObject;
			if (normalInWorldSpace)
				m_hitNormalWorld = convexResult.m_hitNormalLocal;
			else
				m_hitNormalWorld = m_hitCollisionObject->getWorldTransform().getBasis()*convexResult.m_hitNormalLocal;
			m_hitPointWorld = convexResult.m_hitPointLocal;

			// Stop any further searching:
			m_closestHitFraction = btScalar(0.);
			return btScalar(0.);
		}
	};

	class AllHitsConvexResultCallback : public btCollisionWorld::ConvexResultCallback
	{
	public:
		AllHitsConvexResultCallback(Physics3DSweepTest_Impl *impl) : impl(impl)
		{
		}

		virtual	btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
		{
			SweepHit hit;
			hit.hit_fraction = convexResult.m_hitFraction;
			hit.hit_collision_object = static_cast<Physics3DObject_Impl*>(convexResult.m_hitCollisionObject->getUserPointer());

			if (normalInWorldSpace)
			{
				hit.hit_normal = to_vec3f(convexResult.m_hitNormalLocal);
			}
			else
			{
				///need to transform normal into worldspace
				hit.hit_normal = to_vec3f(convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal);
			}

			//hit.hit_point = to_vec3f(convexResult.m_hitPointLocal);

			impl->hits.push_back(hit);

			return btScalar(1.);
		}

	private:
		static uicore::Vec3f to_vec3f(const btVector3 &v)
		{
			return uicore::Vec3f(v.getX(), v.getY(), v.getZ());
		}

		Physics3DSweepTest_Impl *impl;
	};
};
