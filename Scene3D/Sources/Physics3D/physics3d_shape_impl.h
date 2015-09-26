
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <vector>

class Physics3DShape_Impl
{
public:
	Physics3DShape_Impl();
	~Physics3DShape_Impl();

	std::unique_ptr<btCollisionShape> shape;

	std::shared_ptr<Physics3DShape_Impl> base_model;

	std::vector<uicore::Vec3f> model_vertices;
	std::vector<int> model_elements;
	std::unique_ptr<btTriangleIndexVertexArray> vertex_array;

	std::vector<float> terrain_heights;
};
