
#include "precomp.h"
#include "materials_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/app_model.h"

using namespace clan;

MaterialsController::MaterialsController()
{
	view->style()->set("flex-direction: row");

	materials = std::make_shared<RolloutView>("MATERIALS");
	material = std::make_shared<RolloutView>("MATERIAL");

	view->add_subview(materials);
	view->add_subview(material);

	materials_list = std::make_shared<RolloutList>();
	two_sided_property = std::make_shared<RolloutTextFieldProperty>("TWO SIDED");
	alpha_test_property = std::make_shared<RolloutTextFieldProperty>("ALPHA TEST");
	transparent_property = std::make_shared<RolloutTextFieldProperty>("TRANSPARENT");

	materials_list->set_allow_edit(false);

	materials->content->add_subview(materials_list);

	material->content->add_subview(two_sided_property);
	material->content->add_subview(alpha_test_property);
	material->content->add_subview(transparent_property);

	slots.connect(materials_list->sig_selection_changed(), this, &MaterialsController::materials_list_selection_changed);
	slots.connect(materials_list->sig_selection_clicked(), this, &MaterialsController::materials_list_selection_clicked);
	slots.connect(two_sided_property->sig_value_changed(), this, &MaterialsController::two_sided_property_value_changed);
	slots.connect(alpha_test_property->sig_value_changed(), this, &MaterialsController::alpha_test_property_value_changed);
	slots.connect(transparent_property->sig_value_changed(), this, &MaterialsController::transparent_property_value_changed);

	slots.connect(AppModel::instance()->sig_load_finished, [this]() { update_materials(); });

	update_materials();
}

void MaterialsController::update_materials()
{
	materials_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	if (AppModel::instance()->fbx)
	{
		for (const auto &mesh_mat : AppModel::instance()->fbx->material_names())
		{
			auto item = materials_list->add_item(mesh_mat);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[mesh_mat] = item;
		}
	}

	for (const auto &material : AppModel::instance()->desc.materials)
	{
		auto &item = items[material.mesh_material];
		if (!item)
			item = materials_list->add_item(material.mesh_material);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}

	if (!materials_list->selection())
		material->set_hidden(true);
}

int MaterialsController::get_select_item_index()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &materials = AppModel::instance()->desc.materials;
		for (size_t i = 0; i < materials.size(); i++)
		{
			if (materials[i].mesh_material == name)
				return (int)i;
		}
	}
	return -1;
}

void MaterialsController::update_material_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		material->set_hidden(false);

		const auto &material = AppModel::instance()->desc.materials[index];
		two_sided_property->text_field->set_text(material.two_sided ? "1" : "0");
		alpha_test_property->text_field->set_text(material.alpha_test ? "1" : "0");
		transparent_property->text_field->set_text(material.transparent ? "1" : "0");
	}
	else
	{
		material->set_hidden(true);
	}
}

void MaterialsController::materials_list_selection_changed()
{
	update_material_fields();
}

void MaterialsController::materials_list_selection_clicked()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		int index = get_select_item_index();
		if (index == -1)
		{
			FBXMaterial material;
			material.mesh_material = selection->text();
			AppModel::instance()->undo_system.execute<AddMaterialCommand>(material);

			selection->set_bold(true);
			update_material_fields();
		}
	}
}

void MaterialsController::two_sided_property_value_changed()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto material = app_model->desc.materials.at(selection->index);
		material.two_sided = two_sided_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateMaterialCommand>(selection->index, material);
	}
}

void MaterialsController::alpha_test_property_value_changed()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto material = app_model->desc.materials.at(selection->index);
		material.alpha_test = alpha_test_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateMaterialCommand>(selection->index, material);
	}
}

void MaterialsController::transparent_property_value_changed()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto material = app_model->desc.materials.at(selection->index);
		material.transparent = alpha_test_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateMaterialCommand>(selection->index, material);
	}
}
