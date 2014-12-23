
#include "precomp.h"
#include "materials_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/app_model.h"

using namespace clan;

MaterialsController::MaterialsController()
{
	view->box_style.set_layout_block();

	materials = std::make_shared<RolloutView>("MATERIALS");
	material = std::make_shared<RolloutView>("MATERIAL");

	view->add_subview(materials);
	view->add_subview(material);

	materials_list = std::make_shared<RolloutList>();
	two_sided_property = std::make_shared<RolloutTextFieldProperty>("TWO SIDED");
	alpha_test_property = std::make_shared<RolloutTextFieldProperty>("ALPHA TEST");
	mesh_material_property = std::make_shared<RolloutTextFieldProperty>("MESH MATERIAL");

	materials->content->add_subview(materials_list);

	material->content->add_subview(two_sided_property);
	material->content->add_subview(alpha_test_property);
	material->content->add_subview(mesh_material_property);

	slots.connect(materials_list->sig_selection_changed(), this, &MaterialsController::materials_list_selection_changed);
	slots.connect(materials_list->sig_edit_saved(), this, &MaterialsController::materials_list_edit_saved);
	slots.connect(two_sided_property->sig_value_changed(), this, &MaterialsController::two_sided_property_value_changed);
	slots.connect(alpha_test_property->sig_value_changed(), this, &MaterialsController::alpha_test_property_value_changed);
	slots.connect(mesh_material_property->sig_value_changed(), this, &MaterialsController::mesh_material_property_value_changed);

	slots.connect(AppModel::instance()->sig_load_finished, [this]() { update_materials(); });

	update_materials();
}

void MaterialsController::update_materials()
{
	materials_list->clear();
	bool first = true;
	for (const auto &material : AppModel::instance()->desc.materials)
	{
		auto item = materials_list->add_item(material.name);
		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	materials_list->add_item("");
}

void MaterialsController::update_material_fields()
{
	auto selection = materials_list->selection();
	if (selection && selection->index < AppModel::instance()->desc.materials.size())
	{
		material->set_hidden(false);

		const auto &material = AppModel::instance()->desc.materials[selection->index];
		two_sided_property->text_field->set_text(material.two_sided ? "1" : "0");
		alpha_test_property->text_field->set_text(material.alpha_test ? "1" : "0");
		mesh_material_property->text_field->set_text(material.mesh_material);
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

void MaterialsController::materials_list_edit_saved()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		if (selection->index >= AppModel::instance()->desc.materials.size())
		{
			AppModel::instance()->desc.materials.resize(selection->index + 1);
			materials_list->add_item("");
		}

		auto &material = AppModel::instance()->desc.materials[selection->index];
		material.name = selection->text();

		update_material_fields();
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

void MaterialsController::mesh_material_property_value_changed()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		auto app_model = AppModel::instance();
		auto material = app_model->desc.materials.at(selection->index);
		material.mesh_material = mesh_material_property->text_field->text();
		app_model->undo_system.execute<UpdateMaterialCommand>(selection->index, material);
	}
}
