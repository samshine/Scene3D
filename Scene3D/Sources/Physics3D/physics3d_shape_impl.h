
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <vector>

class Physics3DShapeImpl
{
public:
	Physics3DShapeImpl();
	~Physics3DShapeImpl();

	std::unique_ptr<btCollisionShape> shape;

	std::shared_ptr<Physics3DShapeImpl> base_model;

	std::vector<uicore::Vec3f> model_vertices;
	std::vector<int> model_elements;
	std::unique_ptr<btTriangleIndexVertexArray> vertex_array;

	std::vector<float> terrain_heights;
};
