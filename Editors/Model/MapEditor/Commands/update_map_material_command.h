
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateMapMaterialCommand : public ModelCommand
{
public:
	UpdateMapMaterialCommand(size_t index, MapDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	MapDescMaterial old_material;
	MapDescMaterial new_material;
};
