
#pragma once

class RolloutHeader;

class RolloutView : public clan::View
{
public:
	RolloutView(const std::string &title);

	std::shared_ptr<RolloutHeader> header;
	std::shared_ptr<View> content;

	static const float label_width;

private:
};
