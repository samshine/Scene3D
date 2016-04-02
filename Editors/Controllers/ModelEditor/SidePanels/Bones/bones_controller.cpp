
#include "precomp.h"
#include "bones_controller.h"
#include "Views/ModelEditor/bones_sidepanel.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

BonesController::BonesController()
{
	panel = view->content_view()->add_child<BonesSidePanelView>();

	slots.connect(panel->bones_list->sig_selection_changed(), this, &BonesController::bones_list_selection_changed);
	slots.connect(panel->bones_list->sig_edit_saved(), this, &BonesController::bones_list_edit_saved);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_bones(); });

	update_bones();
}

void BonesController::update_bones()
{
	panel->bones_list->clear();
	for (const auto &bone : ModelAppModel::instance()->desc.bones)
	{
		panel->bones_list->add_item(bone.name);
	}
	panel->bones_list->add_item("");
	panel->bones_list->set_selected(0);
	update_bone_fields();
}

void BonesController::update_bone_fields()
{
	auto selection = panel->bones_list->selected_item();
	if (selection != -1 && selection < (int)ModelAppModel::instance()->desc.bones.size())
	{
		panel->bone_rollout->set_hidden(false);

		const auto &bone = ModelAppModel::instance()->desc.bones[selection];
	}
	else
	{
		panel->bone_rollout->set_hidden(true);
	}
}

void BonesController::bones_list_selection_changed()
{
	update_bone_fields();
}

void BonesController::bones_list_edit_saved()
{
	auto selection = panel->bones_list->selected_item();
	if (selection != -1)
	{
		if (selection >= (int)ModelAppModel::instance()->desc.bones.size())
		{
			ModelDescBone bone;
			bone.name = panel->bones_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<AddBoneCommand>(bone);

			panel->bones_list->add_item("");
		}
		else
		{
			ModelDescBone bone = ModelAppModel::instance()->desc.bones[selection];
			bone.name = panel->bones_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<UpdateBoneCommand>(selection, bone);
		}

		update_bone_fields();
	}
}
