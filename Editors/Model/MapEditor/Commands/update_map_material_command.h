
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateMapMaterialCommand : public ModelCommand
{
public:
	UpdateMapMaterialCommand(size_t index, clan::MapDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	clan::MapDescMaterial old_material;
	clan::MapDescMaterial new_material;
};
