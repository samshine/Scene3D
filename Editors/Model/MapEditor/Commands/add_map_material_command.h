
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMapMaterialCommand : public ModelCommand
{
public:
	AddMapMaterialCommand(MapDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	MapDescMaterial new_material;
};
