
#pragma once

class Icosahedron
{
public:
	Icosahedron(uicore::GraphicContext &gc, bool use_radius_of_inscribed_sphere = false);

	uicore::VertexArrayVector<uicore::Vec3f> vertices;
	uicore::ElementArrayVector<unsigned int> elements;

	static const int num_vertices = 12;
	static const int num_elements = 20 * 3;
};

inline Icosahedron::Icosahedron(uicore::GraphicContext &gc, bool use_radius_of_inscribed_sphere)
{
	// radius of a circumscribed sphere (one that touches the icosahedron at all vertices)
	float x = 0.525731112119133606f;
	float z = 0.850650808352039932f;

	if (use_radius_of_inscribed_sphere) // tangent to each of the icosahedron's faces
	{
		x *= 1.32316908f;
		z *= 1.32316908f;
	}

	uicore::Vec3f local_vertices[num_vertices] =
	{
		uicore::Vec3f(-x, 0.0f, z),
		uicore::Vec3f(x, 0.0f, z),
		uicore::Vec3f(-x, 0.0f, -z),
		uicore::Vec3f(x, 0.0f, -z),
		uicore::Vec3f(0.0f, z, x),
		uicore::Vec3f(0.0f, z, -x),
		uicore::Vec3f(0.0f, -z, x),
		uicore::Vec3f(0.0f, -z, -x),
		uicore::Vec3f(z, x, 0.0f),
		uicore::Vec3f(-z, x, 0.0f),
		uicore::Vec3f(z, -x, 0.0f),
		uicore::Vec3f(-z, -x, 0.0f)
	};

	unsigned int local_elements[num_elements] =
	{
		0,4,1,
		0,9,4,
		9,5,4,
		4,5,8,
		4,8,1,
		8,10,1,
		8,3,10,
		5,3,8,
		5,2,3,
		2,7,3,
		7,10,3,
		7,6,10,
		7,11,6,
		11,0,6,
		0,1,6,
		6,1,10,
		9,0,11,
		9,11,2,
		9,2,5,
		7,2,11
	};

	vertices = uicore::VertexArrayVector<uicore::Vec3f>(gc, local_vertices, num_vertices);
	elements = uicore::ElementArrayVector<unsigned int>(gc, local_elements, num_elements);
}
