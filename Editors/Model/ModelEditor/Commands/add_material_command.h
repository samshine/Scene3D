
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMaterialCommand : public ModelCommand
{
public:
	AddMaterialCommand(ModelDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	ModelDescMaterial new_material;
};
