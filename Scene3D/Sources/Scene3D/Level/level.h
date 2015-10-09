
#pragma once

#include "Scene3D/LevelData/level_data.h"
#include "level_instance.h"

class ModelMeshVisitor;

class Level
{
public:
	Level(const uicore::GraphicContextPtr &gc, std::shared_ptr<LevelData> level_data);
	void visit(ModelMeshVisitor *visitor, const LevelInstance &instance);

private:
	std::shared_ptr<LevelData> level_data;
};
