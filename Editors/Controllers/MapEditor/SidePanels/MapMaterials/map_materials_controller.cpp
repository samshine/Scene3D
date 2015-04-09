
#include "precomp.h"
#include "map_materials_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace clan;

MapMaterialsController::MapMaterialsController()
{
	materials = std::make_shared<RolloutView>("MATERIALS");
	material = std::make_shared<RolloutView>("MATERIAL");

	content_view()->add_subview(materials);
	content_view()->add_subview(material);

	materials_list = std::make_shared<RolloutList>();
	materials_list->set_allow_edit(false);

	materials->content->add_subview(materials_list);

	slots.connect(materials_list->sig_selection_changed(), this, &MapMaterialsController::materials_list_selection_changed);
	slots.connect(materials_list->sig_selection_clicked(), this, &MapMaterialsController::materials_list_selection_clicked);

	slots.connect(MapAppModel::instance()->sig_load_finished, [this]() { update_materials(); });

	update_materials();
}

void MapMaterialsController::update_materials()
{
	materials_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	/*
	if (MapAppModel::instance()->fbx)
	{
		for (const auto &material_name : MapAppModel::instance()->fbx->material_names())
		{
			auto item = materials_list->add_item(material_name);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[material_name] = item;
		}
	}
	*/
	/*
	for (const auto &material : MapAppModel::instance()->desc.materials)
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
	*/

	if (!materials_list->selection())
		material->set_hidden(true);
}

int MapMaterialsController::get_select_item_index()
{
	/*auto selection = materials_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &materials = MapAppModel::instance()->desc.materials;
		for (size_t i = 0; i < materials.size(); i++)
		{
			if (materials[i].mesh_material == name)
				return (int)i;
		}
	}*/
	return -1;
}

void MapMaterialsController::update_material_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		material->set_hidden(false);

		//const auto &material = MapAppModel::instance()->desc.materials[index];
	}
	else
	{
		material->set_hidden(true);
	}
}

void MapMaterialsController::materials_list_selection_changed()
{
	update_material_fields();
}

void MapMaterialsController::materials_list_selection_clicked()
{
	auto selection = materials_list->selection();
	if (selection)
	{
		/*
		if (selection->index >= MapAppModel::instance()->desc.materials.size())
		{
			MapAppModel::instance()->desc.materials.resize(selection->index + 1);
			materials_list->add_item("");
		}

		auto &material = MapAppModel::instance()->desc.materials[selection->index];
		material.name = selection->text();

		update_material_fields();
		*/
	}
}
