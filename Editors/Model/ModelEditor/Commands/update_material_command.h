
#pragma once

#include "Model/UndoSystem/model_command.h"

class UpdateMaterialCommand : public ModelCommand
{
public:
	UpdateMaterialCommand(size_t index, clan::ModelDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	clan::ModelDescMaterial old_material;
	clan::ModelDescMaterial new_material;
};
