
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_button.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Views/Rollout/rollout_position_property.h"
#include <memory>

class MapObjectRolloutView : public RolloutView
{
public:
	MapObjectRolloutView() : RolloutView("OBJECT")
	{
		id = content->add_subview<RolloutTextFieldProperty>("ID");
		type = content->add_subview<RolloutTextFieldProperty>("TYPE");
		position = content->add_subview<RolloutPositionProperty>("POSITION");
		dir = content->add_subview<RolloutTextFieldProperty>("DIR");
		up = content->add_subview<RolloutTextFieldProperty>("UP");
		tilt = content->add_subview<RolloutTextFieldProperty>("TILT");
		scale = content->add_subview<RolloutTextFieldProperty>("SCALE");
		mesh = content->add_subview<RolloutBrowseFieldProperty>("MODEL");
		fields = content->add_subview<RolloutTextFieldProperty>("FIELDS");
	}

	std::shared_ptr<RolloutTextFieldProperty> id;
	std::shared_ptr<RolloutTextFieldProperty> type;
	std::shared_ptr<RolloutPositionProperty> position;
	std::shared_ptr<RolloutTextFieldProperty> dir;
	std::shared_ptr<RolloutTextFieldProperty> up;
	std::shared_ptr<RolloutTextFieldProperty> tilt;
	std::shared_ptr<RolloutTextFieldProperty> scale;
	std::shared_ptr<RolloutBrowseFieldProperty> mesh;
	std::shared_ptr<RolloutTextFieldProperty> fields;
};
