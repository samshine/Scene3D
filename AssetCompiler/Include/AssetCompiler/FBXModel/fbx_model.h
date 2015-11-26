
#pragma once

#include <memory>
#include <string>
#include <vector>

class ModelData;
class ModelDesc;
class MapDesc;
class FBXModelImpl;

class FBXModel
{
public:
	static std::shared_ptr<FBXModel> load(const std::string &filename);

	virtual const std::vector<std::string> &material_names() const = 0;
	virtual const std::vector<std::string> &bone_names() const = 0;
	virtual const std::vector<std::string> &light_names() const = 0;
	virtual const std::vector<std::string> &camera_names() const = 0;

	virtual std::shared_ptr<ModelData> convert(const ModelDesc &desc) = 0;
};

typedef std::shared_ptr<FBXModel> FBXModelPtr;
