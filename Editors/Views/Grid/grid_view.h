
#pragma once

#include <array>

class GridView : public uicore::View
{
public:
	GridView();

	void set_cell(int column, int row, const std::shared_ptr<uicore::View> view);

	void normalize();
	void maximize(int column, int row);

	void layout_children(const uicore::CanvasPtr &canvas) override;
	float calculate_preferred_width(const uicore::CanvasPtr &canvas) override;
	float calculate_preferred_height(const uicore::CanvasPtr &canvas, float width) override;

private:
	const float splitter_size = 5.0f;
	const int cell_width = 2;
	const int cell_height = 2;
	std::vector<std::shared_ptr<uicore::View>> cells;

	int maximized_column = -1;
	int maximized_row = -1;
};
