
#include "precomp.h"
#include "Physics3D/physics3d_contact_pair_test.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_contact_pair_test_impl.h"
#include "physics3d_world_impl.h"

using namespace uicore;

std::shared_ptr<Physics3DContactPairTest> Physics3DContactPairTest::create(const Physics3DWorldPtr &world)
{
	return std::make_shared<Physics3DContactPairTestImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
}

/////////////////////////////////////////////////////////////////////////////

Physics3DContactPairTestImpl::Physics3DContactPairTestImpl(Physics3DWorldImpl *world)
	: world(world)
{
}

Physics3DContactPairTestImpl::~Physics3DContactPairTestImpl()
{
}

bool Physics3DContactPairTestImpl::test(const Physics3DObjectPtr &object_a, const Physics3DObjectPtr &object_b)
{
	throw Exception("Physics3DContactPairTest::test not implemented");
}
