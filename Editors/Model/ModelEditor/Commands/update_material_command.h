
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateMaterialCommand : public ModelCommand
{
public:
	UpdateMaterialCommand(size_t index, ModelDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	ModelDescMaterial old_material;
	ModelDescMaterial new_material;
};
