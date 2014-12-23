
#pragma once

#include "model_command.h"

class UpdateMaterialCommand : public ModelCommand
{
public:
	UpdateMaterialCommand(size_t index, clan::FBXMaterial material);

	void execute() override;
	void rollback() override;

private:
	size_t index;
	clan::FBXMaterial old_material;
	clan::FBXMaterial new_material;
};
