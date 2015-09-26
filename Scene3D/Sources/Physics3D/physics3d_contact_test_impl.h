
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <map>

class Physics3DObject_Impl;
class Physics3DWorld_Impl;

class Physics3DContactTest_Impl
{
public:
	Physics3DContactTest_Impl(Physics3DWorld_Impl *world);
	~Physics3DContactTest_Impl();

	Physics3DWorld_Impl *world;

	struct Contact
	{
		Contact(Physics3DObject_Impl *object, const btVector3 &hit_point, const btVector3 &hit_normal, float hit_distance) : object(object), hit_position(hit_point), hit_normal(hit_normal), hit_distance(hit_distance) { }

		Physics3DObject_Impl *object;
		btVector3 hit_position;
		btVector3 hit_normal;
		float hit_distance;
	};

	std::vector<Contact> contacts;

	class AllHitsContactResultCallback : public btCollisionWorld::ContactResultCallback
	{
	public:
		AllHitsContactResultCallback(Physics3DContactTest_Impl *impl, btCollisionObject *test_object) : impl(impl), test_object(test_object)
		{
		}

		btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1)
		{
			if (colObj0Wrap->getCollisionObject() == test_object)
			{
				impl->contacts.push_back(Contact(static_cast<Physics3DObject_Impl*>(colObj1Wrap->getCollisionObject()->getUserPointer()), cp.getPositionWorldOnA(), -cp.m_normalWorldOnB, cp.getDistance()));
			}
			else
			{
				impl->contacts.push_back(Contact(static_cast<Physics3DObject_Impl*>(colObj0Wrap->getCollisionObject()->getUserPointer()), cp.getPositionWorldOnB(), cp.m_normalWorldOnB, cp.getDistance()));
			}

			return btScalar(1.);
		}

	private:
		Physics3DContactTest_Impl *impl;
		btCollisionObject *test_object;
	};
};
