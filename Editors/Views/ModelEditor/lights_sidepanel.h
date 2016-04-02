
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"

class LightsSidePanelView : public uicore::ColumnView
{
public:
	LightsSidePanelView()
	{
		lights = add_child<RolloutView>("LIGHTS");
		lights_list = lights->content->add_child<RolloutList>();
		lights_list->set_allow_edit(false);

		light = add_child<RolloutView>("LIGHT");
	}

	std::shared_ptr<RolloutView> lights;
	std::shared_ptr<RolloutList> lights_list;

	std::shared_ptr<RolloutView> light;
};
