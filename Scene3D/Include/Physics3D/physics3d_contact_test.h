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


#pragma once

#include <memory>

namespace clan
{
/// \addtogroup clanPhysics3D_World clanPhysics3D World
/// \{

class Physics3DWorld;
class Physics3DObject;
class Physics3DShape;
class Physics3DContactTest_Impl;

class Physics3DContactTest
{
public:
	Physics3DContactTest();
	Physics3DContactTest(Physics3DWorld &world);

	bool is_null() const;

	bool test(const Physics3DObject &object);
	bool test(const Physics3DShape &shape, const Vec3f &position, const Quaternionf &orientation);

	int get_hit_count() const;
	Physics3DObject get_hit_object(int index) const;
	Vec3f get_hit_position(int index) const;
	Vec3f get_hit_normal(int index) const;
	float get_hit_distance(int index) const;

private:
	std::shared_ptr<Physics3DContactTest_Impl> impl;
};

}

/// \}
