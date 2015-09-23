
#pragma once

namespace uicore
{
	class VertexMapping
	{
	public:
		VertexMapping() : vertex_index(), next(nullptr), bone_selectors(255) { }
		~VertexMapping() { delete next; }

		VertexMapping *next;

		Vec3f position;
		Vec3f normal;
		Vec3f tangent;
		Vec3f bitangent;

		Vec4ub color;

		Vec2f diffuse_uv;
		Vec2f specular_uv;
		Vec2f normal_uv;
		Vec2f emission_uv;

		Vec4ub bone_selectors;
		Vec4ub bone_weights;

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
}
