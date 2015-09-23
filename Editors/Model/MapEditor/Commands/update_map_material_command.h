
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateMapMaterialCommand : public ModelCommand
{
public:
	UpdateMapMaterialCommand(size_t index, uicore::MapDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	uicore::MapDescMaterial old_material;
	uicore::MapDescMaterial new_material;
};
