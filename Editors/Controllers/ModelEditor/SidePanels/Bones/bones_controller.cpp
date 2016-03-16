
#include "precomp.h"
#include "bones_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

BonesController::BonesController()
{
	bones = std::make_shared<RolloutView>("BONES");
	bone = std::make_shared<RolloutView>("BONE");

	view->content_view()->add_child(bones);
	view->content_view()->add_child(bone);

	bones_list = std::make_shared<RolloutList>();
	bones_list->set_allow_edit(false);

	bones->content->add_child(bones_list);

	slots.connect(bones_list->sig_selection_changed(), this, &BonesController::bones_list_selection_changed);
	slots.connect(bones_list->sig_selection_clicked(), this, &BonesController::bones_list_selection_clicked);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_bones(); });

	update_bones();
}

void BonesController::update_bones()
{
	bones_list->clear();
	bool first = true;

	if (ModelAppModel::instance()->fbx)
	{
		for (const auto &bone_name : ModelAppModel::instance()->fbx->bone_names())
		{
			bones_list->add_item(bone_name);
		}
	}
	/*
	for (const auto &bone : ModelAppModel::instance()->desc.bones)
	{
		auto &item = items[bone.mesh_bone];
		if (!item)
			item = bones_list->add_item(bone.mesh_bone);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	if (bones_list->selected_item() == -1)
		bone->set_hidden(true);
}

int BonesController::get_select_item_index()
{
	/*auto selection = bones_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &bones = ModelAppModel::instance()->desc.bones;
		for (size_t i = 0; i < bones.size(); i++)
		{
			if (bones[i].mesh_bone == name)
				return (int)i;
		}
	}*/
	return -1;
}

void BonesController::update_bone_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		bone->set_hidden(false);

		//const auto &bone = ModelAppModel::instance()->desc.bones[index];
	}
	else
	{
		bone->set_hidden(true);
	}
}

void BonesController::bones_list_selection_changed()
{
	update_bone_fields();
}

void BonesController::bones_list_selection_clicked()
{
	auto selection = bones_list->selected_item();
	if (selection != -1)
	{
		/*
		if (selection->index >= ModelAppModel::instance()->desc.bones.size())
		{
			ModelAppModel::instance()->desc.bones.resize(selection->index + 1);
			bones_list->add_item("");
		}

		auto &bone = ModelAppModel::instance()->desc.bones[selection->index];
		bone.name = selection->text();

		update_bone_fields();
		*/
	}
}
