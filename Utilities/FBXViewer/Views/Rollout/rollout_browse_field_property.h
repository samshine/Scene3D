
#pragma once

class RolloutView;

class RolloutBrowseFieldProperty : public clan::View
{
public:
	RolloutBrowseFieldProperty(const std::string &label);

	std::shared_ptr<clan::LabelView> label;
	std::shared_ptr<clan::LabelView> browse_field;

	clan::Signal<void()> &sig_browse() { return _sig_browse; }

private:
	clan::Signal<void()> _sig_browse;
};
