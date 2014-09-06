/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#include "precomp.h"
#include "Physics3D/physics3d_contact_pair_test.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_contact_pair_test_impl.h"
#include "physics3d_world_impl.h"

namespace clan
{

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


}
