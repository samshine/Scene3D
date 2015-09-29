
#pragma once

#include <memory>

class ModelData;

class TextureBuilder
{
public:
	static void build(std::shared_ptr<ModelData> model_data, const std::string &output_path);
};
