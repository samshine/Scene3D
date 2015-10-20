
#pragma once

#include <string>
#include <memory>

class ModelData;
class TerrainData;
class Physics3DWorld;

class Physics3DShape
{
public:
	static std::shared_ptr<Physics3DShape> box(const uicore::Vec3f &size);
	static std::shared_ptr<Physics3DShape> capsule(float radius, float height);
	static std::shared_ptr<Physics3DShape> sphere(float radius);
	static std::shared_ptr<Physics3DShape> terrain(const std::shared_ptr<TerrainData> &terrain_data, uicore::Mat4f &out_transform);
	static std::shared_ptr<Physics3DShape> model(const std::shared_ptr<ModelData> &model_data);
	static std::shared_ptr<Physics3DShape> scale_model(const std::shared_ptr<Physics3DShape> &base_model, uicore::Vec3f scale);
	static std::shared_ptr<Physics3DShape> gimpact_model(const std::shared_ptr<ModelData> &model_data);

	virtual ~Physics3DShape() { }
};

typedef std::shared_ptr<Physics3DShape> Physics3DShapePtr;
