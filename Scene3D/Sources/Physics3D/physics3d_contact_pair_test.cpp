
#include "precomp.h"
#include "Physics3D/physics3d_contact_pair_test.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_contact_pair_test_impl.h"
#include "physics3d_world_impl.h"

using namespace uicore;

Physics3DContactPairTest::Physics3DContactPairTest()
{
}

Physics3DContactPairTest::Physics3DContactPairTest(Physics3DWorld &world)
	: impl(std::make_shared<Physics3DContactPairTest_Impl>(world.impl.get()))
{
}

bool Physics3DContactPairTest::is_null() const
{
	return !impl;
}

bool Physics3DContactPairTest::test(const Physics3DObject &object_a, const Physics3DObject &object_b)
{
	throw Exception("Physics3DContactPairTest::test not implemented");
}

/////////////////////////////////////////////////////////////////////////////

Physics3DContactPairTest_Impl::Physics3DContactPairTest_Impl(Physics3DWorld_Impl *world)
	: world(world)
{
}

Physics3DContactPairTest_Impl::~Physics3DContactPairTest_Impl()
{
}
