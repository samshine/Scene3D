
#pragma once

#include "model_data_draw_range.h"

/// \brief Vertex attributes and draw ranges required to draw a mesh
class ModelDataMesh
{
public:
	std::vector<uicore::Vec3f> vertices;
	std::vector<uicore::Vec3f> normals;
	std::vector<uicore::Vec4ub> bone_weights;
	std::vector<uicore::Vec4ub> bone_selectors;
	std::vector<uicore::Vec4ub> colors;
	std::vector<std::vector<uicore::Vec2f>> channels;
	std::vector<unsigned int> elements;
	std::vector<ModelDataDrawRange> draw_ranges;
};
