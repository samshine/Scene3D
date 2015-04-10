
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMapMaterialCommand : public ModelCommand
{
public:
	AddMapMaterialCommand(clan::MapDescMaterial material);

	void execute() override;
	void rollback() override;

private:
	clan::MapDescMaterial new_material;
};
