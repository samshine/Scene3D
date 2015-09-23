
#pragma once

class RolloutView;

class RolloutBrowseFieldProperty : public uicore::View
{
public:
	RolloutBrowseFieldProperty(const std::string &label);

	std::shared_ptr<uicore::LabelView> label;
	std::shared_ptr<uicore::LabelView> browse_field;

	uicore::Signal<void()> &sig_browse() { return _sig_browse; }

private:
	uicore::Signal<void()> _sig_browse;
};
