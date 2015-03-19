
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace clan
{
	class ModelData;
	class ModelDesc;
	class FBXModelImpl;

	class FBXModel
	{
	public:
		FBXModel(const std::string &filename);

		const std::vector<std::string> &material_names() const;
		const std::vector<std::string> &bone_names() const;
		const std::vector<std::string> &light_names() const;
		const std::vector<std::string> &camera_names() const;

		std::shared_ptr<ModelData> convert(const ModelDesc &desc, bool bake_light = false);

	private:
		std::shared_ptr<FBXModelImpl> impl;
	};
}
