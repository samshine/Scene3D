
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMaterialCommand : public ModelCommand
{
public:
	AddMaterialCommand(uicore::ModelDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	uicore::ModelDescMaterial new_material;
};
