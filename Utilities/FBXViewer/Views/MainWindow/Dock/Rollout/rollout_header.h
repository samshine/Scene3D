
#pragma once

class RolloutHeader : public clan::View
{
public:
	RolloutHeader();
	void set_text(const std::string &text);

private:
	std::shared_ptr<clan::LabelView> label;
};
