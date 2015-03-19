
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMaterialCommand : public ModelCommand
{
public:
	AddMaterialCommand(clan::ModelDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	clan::ModelDescMaterial new_material;
};
