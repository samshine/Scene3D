
#pragma once

class RolloutHeader : public uicore::View
{
public:
	RolloutHeader();
	void set_text(const std::string &text);

private:
	std::shared_ptr<uicore::LabelBaseView> label;
};
