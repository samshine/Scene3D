
#pragma once

#include "Model/UndoSystem/model_command.h"

class AddMaterialCommand : public ModelCommand
{
public:
	AddMaterialCommand(clan::FBXMaterial material);

	void execute() override;
	void rollback() override;

private:
	clan::FBXMaterial new_material;
};
