
#include "precomp.h"
#include "materials_controller.h"
#include "Views/ModelEditor/materials_sidepanel.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

MaterialsController::MaterialsController()
{
	panel = view->content_view()->add_child<MaterialsSidePanelView>();

	slots.connect(panel->materials_list->sig_selection_changed(), this, &MaterialsController::materials_list_selection_changed);
	slots.connect(panel->materials_list->sig_selection_clicked(), this, &MaterialsController::materials_list_selection_clicked);
	slots.connect(panel->two_sided_property->sig_value_changed(), this, &MaterialsController::two_sided_property_value_changed);
	slots.connect(panel->alpha_test_property->sig_value_changed(), this, &MaterialsController::alpha_test_property_value_changed);
	slots.connect(panel->transparent_property->sig_value_changed(), this, &MaterialsController::transparent_property_value_changed);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_materials(); });

	update_materials();
}

void MaterialsController::update_materials()
{
	panel->materials_list->clear();
	/*
	bool first = true;

	if (ModelAppModel::instance()->fbx)
	{
		for (const auto &mesh_mat : ModelAppModel::instance()->fbx->material_names())
		{
			panel->materials_list->add_item(mesh_mat);
		}
	}

	int index = 0;
	for (const auto &material : ModelAppModel::instance()->desc.materials)
	{
		panel->materials_list->add_item(material.mesh_material);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true);
			first = false;
		}
		index++;
	}

	if (index > 0)
		panel->materials_list->set_selected(0);

	if (panel->materials_list->selected_item() == -1)*/
		panel->material->set_hidden(true);
}

int MaterialsController::get_select_item_index()
{
	auto selection = panel->materials_list->selected_item();
	if (selection != -1)
	{
		std::string name = panel->materials_list->item_text(selection);

		const auto &materials = ModelAppModel::instance()->desc.materials;
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
		panel->material->set_hidden(false);

		const auto &material = ModelAppModel::instance()->desc.materials[index];
		panel->two_sided_property->text_field->set_text(material.two_sided ? "1" : "0");
		panel->alpha_test_property->text_field->set_text(material.alpha_test ? "1" : "0");
		panel->transparent_property->text_field->set_text(material.transparent ? "1" : "0");
	}
	else
	{
		panel->material->set_hidden(true);
	}
}

void MaterialsController::materials_list_selection_changed()
{
	update_material_fields();
}

void MaterialsController::materials_list_selection_clicked()
{
	auto selection = panel->materials_list->selected_item();
	if (selection != -1)
	{
		int index = get_select_item_index();
		if (index == -1)
		{
			ModelDescMaterial material;
			material.mesh_material = panel->materials_list->item_text(selection);
			ModelAppModel::instance()->undo_system.execute<AddMaterialCommand>(material);

			panel->materials_list->set_bold(selection);
			update_material_fields();
		}
	}
}

void MaterialsController::two_sided_property_value_changed()
{
	auto selection = panel->materials_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto material = app_model->desc.materials.at(selection);
		material.two_sided = panel->two_sided_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateMaterialCommand>(selection, material);
	}
}

void MaterialsController::alpha_test_property_value_changed()
{
	auto selection = panel->materials_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto material = app_model->desc.materials.at(selection);
		material.alpha_test = panel->alpha_test_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateMaterialCommand>(selection, material);
	}
}

void MaterialsController::transparent_property_value_changed()
{
	auto selection = panel->materials_list->selected_item();
	if (selection != -1)
	{
		auto app_model = ModelAppModel::instance();
		auto material = app_model->desc.materials.at(selection);
		material.transparent = panel->alpha_test_property->text_field->text_int() == 1;
		app_model->undo_system.execute<UpdateMaterialCommand>(selection, material);
	}
}
