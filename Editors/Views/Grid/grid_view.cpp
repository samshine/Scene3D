
#include "precomp.h"
#include "grid_view.h"

using namespace uicore;

GridView::GridView()
{
	style()->set("flex: auto");
	style()->set("background: rgb(47,75,99)");
	cells.resize(cell_width * cell_height);
}

void GridView::normalize()
{
	maximized_column = -1;
	maximized_row = -1;
	for (auto &cell : cells)
		cell->set_hidden(false);
	set_needs_layout();
}

void GridView::maximize(int maxi_column, int maxi_row)
{
	if (maxi_column == -1 || maxi_row == -1)
	{
		normalize();
		return;
	}

	maximized_column = maxi_column;
	maximized_row = maxi_row;

	for (int row = 0; row < cell_height; row++)
	{
		for (int col = 0; col < cell_width; col++)
		{
			auto &cell = cells[col + row * cell_width];
			if (cell)
				cell->set_hidden(col != maxi_column || row != maxi_row);
		}
	}

	set_needs_layout();
}

void GridView::set_cell(int column, int row, const std::shared_ptr<uicore::View> view)
{
	auto &cell = cells[column + row * cell_width];
	if (cell)
	{
		cell->remove_from_super();
		cell = nullptr;
	}

	add_subview(view);
	cell = view;
	set_needs_layout();
}

void GridView::layout_subviews(const CanvasPtr &canvas)
{
	if (maximized_row != -1 && maximized_column != -1)
	{
		auto &cell = cells[maximized_column + maximized_row * cell_width];
		if (cell)
			cell->set_geometry(ViewGeometry::from_margin_box(cell->style_cascade(), geometry().content_size()));
	}
	else
	{
		float nonsplitter_width = std::max(geometry().content_width - splitter_size * (cell_width - 1), 0.0f);
		float nonsplitter_height = std::max(geometry().content_height - splitter_size * (cell_height - 1), 0.0f);

		float y = 0.0f;
		for (int row = 0; row < cell_height; row++)
		{
			float x = 0.0f;
			for (int col = 0; col < cell_width; col++)
			{
				auto box = Rectf::xywh(x, y, nonsplitter_width / cell_width, nonsplitter_height / cell_height);
				auto &cell = cells[col + row * cell_width];
				if (cell)
					cell->set_geometry(ViewGeometry::from_margin_box(cell->style_cascade(), box));

				x += nonsplitter_width / cell_width + splitter_size;
			}
			y += nonsplitter_height / cell_height + splitter_size;
		}
	}
}

float GridView::calculate_preferred_width(const CanvasPtr &canvas)
{
	float max_width = 0.0f;
	for (int row = 0; row < cell_height; row++)
	{
		float width = 0;
		for (int col = 0; col < cell_width; col++)
		{
			auto &cell = cells[col + row * cell_width];
			if (cell)
				width += cell->preferred_width(canvas);
		}
		max_width = std::max(max_width, width);
	}
	return splitter_size * (cell_width - 1) + max_width;
}

float GridView::calculate_preferred_height(const CanvasPtr &canvas, float width)
{
	float total_width = std::max(width - splitter_size * (cell_width - 1), 0.0f);

	float height = 0.0f;
	for (int row = 0; row < cell_height; row++)
	{
		float row_height = 0;
		for (int col = 0; col < cell_width; col++)
		{
			auto &cell = cells[col + row * cell_width];
			if (cell)
				row_height = std::max(cell->preferred_height(canvas, total_width / cell_width), row_height);
		}
		height += row_height;
	}
	return splitter_size * (cell_height - 1) + height;
}
