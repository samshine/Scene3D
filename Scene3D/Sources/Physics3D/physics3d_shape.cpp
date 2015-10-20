
#include "precomp.h"
#include "Physics3D/physics3d_shape.h"
#include "Scene3D/ModelData/model_data.h"
#include "Physics3D/Bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "Physics3D/Bullet/BulletCollision/Gimpact/btGImpactShape.h"
#include "physics3d_shape_impl.h"

std::shared_ptr<Physics3DShape> Physics3DShape::box(const uicore::Vec3f &size)
{
	auto shape = std::make_shared<Physics3DShapeImpl>();
	shape->shape.reset(new btBoxShape(btVector3(size.x, size.y, size.z)));
	shape->shape->setUserPointer(shape.get());
	return shape;
}

std::shared_ptr<Physics3DShape> Physics3DShape::capsule(float radius, float height)
{
	auto shape = std::make_shared<Physics3DShapeImpl>();
	shape->shape.reset(new btCapsuleShape(radius, height));
	shape->shape->setUserPointer(shape.get());
	return shape;
}

std::shared_ptr<Physics3DShape> Physics3DShape::sphere(float radius)
{
	auto shape = std::make_shared<Physics3DShapeImpl>();
	shape->shape.reset(new btSphereShape(radius));
	shape->shape->setUserPointer(shape.get());
	return shape;
}

std::shared_ptr<Physics3DShape> Physics3DShape::model(const std::shared_ptr<ModelData> &model_data)
{
	auto shape = std::make_shared<Physics3DShapeImpl>();

	unsigned int vertex_offset = 0;
	for (size_t i = 0; i < model_data->meshes.size(); i++)
	{
		shape->model_vertices.insert(shape->model_vertices.end(), model_data->meshes[i].vertices.begin(), model_data->meshes[i].vertices.end());
		for (size_t j = 0; j < model_data->meshes[i].elements.size(); j++)
			shape->model_elements.push_back(vertex_offset + model_data->meshes[i].elements[j]);
		vertex_offset += model_data->meshes[i].vertices.size();
	}

	btIndexedMesh submesh;
	submesh.m_numTriangles = (int)shape->model_elements.size() / 3;
	submesh.m_triangleIndexBase = reinterpret_cast<const unsigned char *>(&shape->model_elements[0]);
	submesh.m_triangleIndexStride = sizeof(int)*3;
	submesh.m_vertexType = PHY_FLOAT;
	submesh.m_vertexBase = reinterpret_cast<const unsigned char *>(&shape->model_vertices[0]);
	submesh.m_vertexStride = sizeof(uicore::Vec3f);
	submesh.m_numVertices = (int)shape->model_vertices.size();

	shape->vertex_array.reset(new btTriangleIndexVertexArray());
	shape->vertex_array->addIndexedMesh(submesh, PHY_INTEGER);

	bool useQuantizedAabbCompression = true;

	// if create from memory:
	btBvhTriangleMeshShape *mesh = new btBvhTriangleMeshShape(shape->vertex_array.get(), useQuantizedAabbCompression);
	shape->shape.reset(mesh);
	shape->shape->setUserPointer(shape.get());
/*
	// if save to disk:

	int max_buffer_size = 1024*1024*5;
	std::unique_ptr<btDefaultSerializer> serializer(new btDefaultSerializer(max_buffer_size));
	serializer->startSerialization();
	mesh->serializeSingleBvh(serializer.get()); // or maybe use serializeSingleShape(serializer.get()) ?
	serializer->finishSerialization();
	File file("model.bullet", File::create_always, File::access_read_write);
	file.write(serializer->getBufferPointer(), serializer->getCurrentBufferSize());
	file.close();

	// if load from disk:

	btBulletWorldImporter import(0);
	if (import.loadFile("model.bullet"))
	{
		std::unique_ptr<btTriangleIndexVertexArray> vertex_array;

		int num_bvh = import.getNumBvhs();
		if (num_bvh)
		{
			btOptimizedBvh* bvh = import.getBvhByIndex(0);
			btBvhTriangleMeshShape *mesh = new btBvhTriangleMeshShape(shape->vertex_array, useQuantizedAabbCompression, aabb_min, aabb_max, false);
			shape->shape = mesh;
			shape->setOptimizedBvh(bvh);
		}
	}
	else
	{
		throw Exception("Bullet mesh not found");
	}
*/

	return shape;
}

std::shared_ptr<Physics3DShape> Physics3DShape::scale_model(const Physics3DShapePtr &base_model, uicore::Vec3f scale)
{
	btBvhTriangleMeshShape *base_shape = dynamic_cast<btBvhTriangleMeshShape*>(static_cast<Physics3DShapeImpl*>(base_model.get())->shape.get());
	if (base_shape == 0)
		throw uicore::Exception("Physics3DShape is not a model shape!");

	auto shape = std::make_shared<Physics3DShapeImpl>();
	shape->base_model = std::dynamic_pointer_cast<Physics3DShapeImpl>(base_model);
	shape->shape.reset(new btScaledBvhTriangleMeshShape(base_shape, btVector3(scale.x, scale.y, scale.z)));
	shape->shape->setUserPointer(shape.get());
	return shape;
}

std::shared_ptr<Physics3DShape> Physics3DShape::gimpact_model(const std::shared_ptr<ModelData> &model_data)
{
	auto shape = std::make_shared<Physics3DShapeImpl>();

	unsigned int vertex_offset = 0;
	for (size_t i = 0; i < model_data->meshes.size(); i++)
	{
		shape->model_vertices.insert(shape->model_vertices.end(), model_data->meshes[i].vertices.begin(), model_data->meshes[i].vertices.end());
		for (size_t j = 0; j < model_data->meshes[i].elements.size(); j++)
			shape->model_elements.push_back(vertex_offset + model_data->meshes[i].elements[j]);
		vertex_offset += model_data->meshes[i].vertices.size();
	}

	btIndexedMesh submesh;
	submesh.m_numTriangles = (int)shape->model_elements.size() / 3;
	submesh.m_triangleIndexBase = reinterpret_cast<const unsigned char *>(&shape->model_elements[0]);
	submesh.m_triangleIndexStride = sizeof(int)*3;
	submesh.m_vertexType = PHY_FLOAT;
	submesh.m_vertexBase = reinterpret_cast<const unsigned char *>(&shape->model_vertices[0]);
	submesh.m_vertexStride = sizeof(uicore::Vec3f);
	submesh.m_numVertices = (int)shape->model_vertices.size();

	shape->vertex_array.reset(new btTriangleIndexVertexArray());
	shape->vertex_array->addIndexedMesh(submesh, PHY_INTEGER);

	shape->shape.reset(new btGImpactMeshShape(shape->vertex_array.get()));
	shape->shape->setUserPointer(shape.get());

	return shape;
}

std::shared_ptr<Physics3DShape> Physics3DShape::terrain(const std::shared_ptr<TerrainData> &terrain_data, uicore::Mat4f &out_transform)
{
	throw uicore::Exception("Physics3DShape::terrain not implemented");
/*
	auto shape = std::make_shared<Physics3DShapeImpl>();
	shape->heights = terrain_data->heights();

	float min_height = shape->heights[0];
	float max_height = shape->heights[0];
	for (size_t i = 1; i < shape->heights.size(); i++)
	{
		min_height = min(min_height, shape->heights[i]);
		max_height = max(max_height, shape->heights[i]);
	}

	int up_axis = 1; // y is up
	shape->shape.reset(new btHeightfieldTerrainShape(tile.width(), tile.height(), &shape->heights[0], 1.0f, min_height, max_height, up_axis, PHY_FLOAT, false));
	shape->shape->setUserPointer(shape.get());
	terrain->setUseDiamondSubdivision(true);

	out_transform = Mat4f::translate(tile.x() + tile.width() * 0.5f, min_height + (max_height - min_height) * 0.5f, tile.y() + tile.height() * 0.5f);
	return shape;
*/
}

/*
std::shared_ptr<Physics3DShape> Physics3DShape::terrain_with_holes(const std::shared_ptr<uicore::TerrainData> &terrain_data, Mat4f &out_transform)
{
	out_transform = Mat4f::translate((float)tile.x(), 0.0f, (float)tile.y());

	auto shape = std::make_shared<Physics3DShapeImpl>();

	const TerrainVector<unsigned char> &flags = tile.flags();
	const TerrainVector<float> &heights = tile.heights();

	std::shared_ptr<ModelData> model_data(new ModelData());
	for (int y = 0; y < tile.height(); y++)
	{
		for (int x = 0;  x < tile.width(); x++)
		{
			shape->model_vertices.push_back(Vec3f((float)x, (float)y, heights[x + y * tile.width()]));
		}

		if (y + 1 != tile.height())
		{
			for (int x = 0;  x < tile.width() - 1; x++)
			{
				int offset = x + y * tile.width();
				float height = 0.25f * (heights[offset] + heights[offset + 1] + heights[offset + tile.width()] + heights[offset + 1 + tile.width()]);
				shape->model_vertices.push_back(Vec3f((float)x + 0.5f, (float)y + 0.5f, height));
			}
		}
	}

	int width = tile.width() - 1;
	int height = tile.height() - 1;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0;  x < width; x++)
		{
			if ((flags[x + y * tile.width()] & ContinentBlock::flag_hole) == 0)
			{
				int index_top_left = x + y * (width * 2 + 1);
				int index_top_right = index_top_left + 1;
				int index_bottom_left = index_top_left + width * 2 + 1;
				int index_bottom_right = index_bottom_left + 1;
				int index_center = index_top_left + width + 1;

				shape->model_elements.push_back(index_top_left);
				shape->model_elements.push_back(index_top_right);
				shape->model_elements.push_back(index_center);

				shape->model_elements.push_back(index_center);
				shape->model_elements.push_back(index_bottom_left);
				shape->model_elements.push_back(index_top_left);

				shape->model_elements.push_back(index_center);
				shape->model_elements.push_back(index_bottom_right);
				shape->model_elements.push_back(index_bottom_left);

				shape->model_elements.push_back(index_top_right);
				shape->model_elements.push_back(index_bottom_right);
				shape->model_elements.push_back(index_center);
			}
		}
	}

	btIndexedMesh submesh;
	submesh.m_numTriangles = (int)shape->model_elements.size() / 3;
	submesh.m_triangleIndexBase = reinterpret_cast<const unsigned char *>(&shape->model_elements[0]);
	submesh.m_triangleIndexStride = sizeof(int)*3;
	submesh.m_vertexType = PHY_FLOAT;
	submesh.m_vertexBase = reinterpret_cast<const unsigned char *>(&shape->model_vertices[0]);
	submesh.m_vertexStride = sizeof(Vec3f);
	submesh.m_numVertices = (int)shape->model_vertices.size();

	shape->vertex_array = new btTriangleIndexVertexArray();
	shape->vertex_array->addIndexedMesh(submesh, PHY_INTEGER);

	bool useQuantizedAabbCompression = true;
	shape->shape.reset(new btBvhTriangleMeshShape(shape->vertex_array, useQuantizedAabbCompression));
	shape->shape->setUserPointer(shape.get());
	return shape;
}
*/
