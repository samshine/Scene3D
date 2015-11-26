
#pragma once

#include "model_instances_buffer.h"
#include "model_shader_cache.h"

class ModelRender
{
public:
	ModelShaderCache shader_cache;
	ModelInstancesBuffer instances_buffer;
};
