
#pragma once

#include <array>

class GridView : public uicore::View
{
public:
	GridView();

	void set_cell(int column, int row, const std::shared_ptr<uicore::View> view);

	void layout_subviews(const uicore::CanvasPtr &canvas) override;
	float calculate_preferred_width(const uicore::CanvasPtr &canvas) override;
	float calculate_preferred_height(const uicore::CanvasPtr &canvas, float width) override;

private:
	const float splitter_size = 5.0f;
	const int cell_width = 2;
	const int cell_height = 2;
	std::vector<std::shared_ptr<uicore::View>> cells;
};
