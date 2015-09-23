
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMapMaterialCommand : public ModelCommand
{
public:
	AddMapMaterialCommand(uicore::MapDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	uicore::MapDescMaterial new_material;
};
