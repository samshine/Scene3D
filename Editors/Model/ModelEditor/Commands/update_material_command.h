
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateMaterialCommand : public ModelCommand
{
public:
	UpdateMaterialCommand(size_t index, uicore::ModelDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	uicore::ModelDescMaterial old_material;
	uicore::ModelDescMaterial new_material;
};
