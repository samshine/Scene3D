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
#include "quad_tree_node.h"

namespace uicore
{

QuadTreeNode::QuadTreeNode()
{
	for (int i = 0; i < 4; i++)
		children[i] = 0;
}

QuadTreeNode::~QuadTreeNode()
{
	for (int i = 0; i < 4; i++)
		delete children[i];
}

void QuadTreeNode::cull(int frame, const FrustumPlanes &frustum, const AxisAlignedBoundingBox &aabb, std::vector<SceneItem *> &pvs)
{
/*	if (frustum.is_visible(aabb))
	{
		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->add(frame, pvs);

		children[0]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.5f, 1.0f, 0.5f)), pvs);
		children[1]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.5f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 0.5f)), pvs);
		children[2]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.0f, 0.0f, 0.5f), Vec3f(0.5f, 1.0f, 1.0f)), pvs);
		children[3]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.5f, 0.0f, 0.5f), Vec3f(1.0f, 1.0f, 1.0f)), pvs);
	}*/
}

}

