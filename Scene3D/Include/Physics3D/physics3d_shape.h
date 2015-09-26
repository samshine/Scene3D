
#pragma once

#include <string>
#include <memory>

class ModelData;
class TerrainData;
class Physics3DWorld;
class Physics3DShape_Impl;

class Physics3DShape
{
public:
	Physics3DShape();

	bool is_null() const;

	static Physics3DShape box(const uicore::Vec3f &size);
	static Physics3DShape capsule(float radius, float height);
	static Physics3DShape sphere(float radius);
	static Physics3DShape terrain(const std::shared_ptr<TerrainData> &terrain_data, uicore::Mat4f &out_transform);
	static Physics3DShape model(const std::shared_ptr<ModelData> &model_data);
	static Physics3DShape scale_model(const Physics3DShape &base_model, uicore::Vec3f scale);
	static Physics3DShape gimpact_model(const std::shared_ptr<ModelData> &model_data);

private:
	std::shared_ptr<Physics3DShape_Impl> impl;

	friend class Physics3DObject;
	friend class Physics3DSweepTest;
	friend class Physics3DContactTest;
};
