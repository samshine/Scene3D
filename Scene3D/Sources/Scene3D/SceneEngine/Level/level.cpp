
#include "precomp.h"
#include "level.h"

using namespace uicore;

Level::Level(const GraphicContextPtr &gc, std::shared_ptr<LevelData> level_data)
: level_data(level_data)
{
}

void Level::visit(ModelMeshVisitor *visitor, const LevelInstance &instance)
{
}
