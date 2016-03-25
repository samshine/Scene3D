
#pragma once

class VertexMapping
{
public:
	VertexMapping() : vertex_index(), next(nullptr), bone_selectors(255) { }
	~VertexMapping() { delete next; }

	VertexMapping *next;

	uicore::Vec3f position;
	uicore::Vec3f normal;

	uicore::Vec4ub color;

	uicore::Vec2f diffuse_uv;
	uicore::Vec2f specular_uv;
	uicore::Vec2f normal_uv;
	uicore::Vec2f emission_uv;

	uicore::Vec4ub bone_selectors;
	uicore::Vec4ub bone_weights;

	int vertex_index;
};

class VertexMappingVector : public std::vector<VertexMapping *>
{
public:
	VertexMappingVector(size_t count)
		: std::vector<VertexMapping *>(count)
	{
	}

	~VertexMappingVector()
	{
		for (size_t i = 0; i < size(); i++)
			delete (*this)[i];
	}

private:
	VertexMappingVector(const VertexMappingVector &that);
	VertexMappingVector &operator=(const VertexMappingVector &that);
};
