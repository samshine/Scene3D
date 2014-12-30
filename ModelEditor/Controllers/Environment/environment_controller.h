
#pragma once

class RolloutView;
class RolloutTextFieldProperty;
class RolloutBrowseFieldProperty;

class EnvironmentController : public clan::ViewController
{
public:
	EnvironmentController();

private:
	void map_model_property_browse();
	void map_model_updated();

	std::shared_ptr<RolloutView> environment;
	std::shared_ptr<RolloutBrowseFieldProperty> map_model_property;

	clan::SlotContainer slots;
};
