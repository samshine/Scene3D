
#pragma once

#include "Physics3D/physics3d_contact_pair_test.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <map>

class Physics3DObjectImpl;
class Physics3DWorldImpl;

class Physics3DContactTestImpl : public Physics3DContactTest
{
public:
	Physics3DContactTestImpl(Physics3DWorldImpl *world);
	~Physics3DContactTestImpl();

	bool test(const Physics3DObject &object) override;
	bool test(const Physics3DShape &shape, const uicore::Vec3f &position, const uicore::Quaternionf &orientation) override;

	int hit_count() const override;
	Physics3DObject hit_object(int index) const override;
	uicore::Vec3f hit_position(int index) const override;
	uicore::Vec3f hit_normal(int index) const override;
	float hit_distance(int index) const override;

private:
	Physics3DWorldImpl *world;

	struct Contact
	{
		Contact(Physics3DObjectImpl *object, const btVector3 &hit_point, const btVector3 &hit_normal, float hit_distance) : object(object), hit_position(hit_point), hit_normal(hit_normal), hit_distance(hit_distance) { }

		Physics3DObjectImpl *object;
		btVector3 hit_position;
		btVector3 hit_normal;
		float hit_distance;
	};

	std::vector<Contact> contacts;

	class AllHitsContactResultCallback : public btCollisionWorld::ContactResultCallback
	{
	public:
		AllHitsContactResultCallback(Physics3DContactTestImpl *impl, btCollisionObject *test_object) : impl(impl), test_object(test_object)
		{
		}

		btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1)
		{
			if (colObj0Wrap->getCollisionObject() == test_object)
			{
				impl->contacts.push_back(Contact(static_cast<Physics3DObjectImpl*>(colObj1Wrap->getCollisionObject()->getUserPointer()), cp.getPositionWorldOnA(), -cp.m_normalWorldOnB, cp.getDistance()));
			}
			else
			{
				impl->contacts.push_back(Contact(static_cast<Physics3DObjectImpl*>(colObj0Wrap->getCollisionObject()->getUserPointer()), cp.getPositionWorldOnB(), cp.m_normalWorldOnB, cp.getDistance()));
			}

			return btScalar(1.);
		}

	private:
		Physics3DContactTestImpl *impl;
		btCollisionObject *test_object;
	};
};
