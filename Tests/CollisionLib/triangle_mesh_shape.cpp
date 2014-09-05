
#include "precomp.h"
#include "triangle_mesh_shape.h"
#include "sphere_shape.h"
#include <algorithm>

TriangleMeshShape::TriangleMeshShape(const Vec3f *vertices, int num_vertices, const unsigned int *elements, int num_elements)
: vertices(vertices), num_vertices(num_vertices), elements(elements), num_elements(num_elements), root(-1)
{
	int num_triangles = num_elements / 3;
	if (num_triangles <= 0)
		return;

	std::vector<int> triangles;
	std::vector<Vec3f> centroids;
	triangles.reserve(num_triangles);
	centroids.reserve(num_triangles);
	for (int i = 0; i < num_triangles; i++)
	{
		triangles.push_back(i);

		int element_index = i * 3;
		Vec3f centroid = (vertices[elements[element_index + 0]] + vertices[elements[element_index + 1]] + vertices[elements[element_index + 2]]) * (1.0f / 3.0f);
		centroids.push_back(centroid);
	}

	std::vector<int> work_buffer(num_triangles * 2);

	root = subdivide(&triangles[0], triangles.size(), &centroids[0], &work_buffer[0]);
}

float TriangleMeshShape::sweep(TriangleMeshShape *shape1, SphereShape *shape2, const Vec3f &target)
{
	return sweep(shape1, shape2, shape1->root, target);
}

bool TriangleMeshShape::find_any_hit(TriangleMeshShape *shape1, TriangleMeshShape *shape2)
{
	return find_any_hit(shape1, shape2, shape1->root, shape2->root);
}

bool TriangleMeshShape::find_any_hit(TriangleMeshShape *shape1, SphereShape *shape2)
{
	return find_any_hit(shape1, shape2, shape1->root);
}

float TriangleMeshShape::sweep(TriangleMeshShape *shape1, SphereShape *shape2, int a, const Vec3f &target)
{
	if (sweep_overlap_bv_sphere(shape1, shape2, a, target))
	{
		if (shape1->is_leaf(a))
		{
			return sweep_intersect_triangle_sphere(shape1, shape2, a, target);
		}
		else
		{
			return std::min(sweep(shape1, shape2, shape1->nodes[a].left, target), sweep(shape1, shape2, shape1->nodes[a].right, target));
		}
	}
	return 1.0f;
}

bool TriangleMeshShape::find_any_hit(TriangleMeshShape *shape1, SphereShape *shape2, int a)
{
	if (overlap_bv_sphere(shape1, shape2, a))
	{
		if (shape1->is_leaf(a))
		{
			return overlap_triangle_sphere(shape1, shape2, a);
		}
		else
		{
			if (find_any_hit(shape1, shape2, shape1->nodes[a].left))
				return true;
			else
				return find_any_hit(shape1, shape2, shape1->nodes[a].right);
		}
	}
	return false;
}

bool TriangleMeshShape::find_any_hit(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b)
{
	bool leaf_a = shape1->is_leaf(a);
	bool leaf_b = shape2->is_leaf(b);
	if (leaf_a && leaf_b)
	{
		return overlap_triangle_triangle(shape1, shape2, a, b);
	}
	else if (!leaf_a && !leaf_b)
	{
		if (overlap_bv(shape1, shape2, a, b))
		{
			if (shape1->volume(a) > shape2->volume(b))
			{
				if (find_any_hit(shape1, shape2, shape1->nodes[a].left, b))
					return true;
				else
					return find_any_hit(shape1, shape2, shape1->nodes[a].right, b);
			}
			else
			{
				if (find_any_hit(shape1, shape2, a, shape2->nodes[b].left))
					return true;
				else
					return find_any_hit(shape1, shape2, a, shape2->nodes[b].right);
			}
		}
		return false;
	}
	else if (leaf_a)
	{
		if (overlap_bv_triangle(shape2, shape1, b, a))
		{
			if (find_any_hit(shape1, shape2, a, shape2->nodes[b].left))
				return true;
			else
				return find_any_hit(shape1, shape2, a, shape2->nodes[b].right);
		}
		return false;
	}
	else
	{
		if (overlap_bv_triangle(shape1, shape2, a, b))
		{
			if (find_any_hit(shape1, shape2, shape1->nodes[a].left, b))
				return true;
			else
				return find_any_hit(shape1, shape2, shape1->nodes[a].right, b);
		}
		return false;
	}
}

bool TriangleMeshShape::sweep_overlap_bv_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int a, const Vec3f &target)
{
	// Convert to ray test by expanding the AABB:

	AxisAlignedBoundingBox aabb = shape1->nodes[a].aabb;
	aabb.aabb_min -= shape2->radius;
	aabb.aabb_max += shape2->radius;

	// IntersectionTest::ray_aabb(o,d,A):

	Vec3f c = (shape2->center + target) * 0.5f;
	Vec3f w = target - c;
	Vec3f h = aabb.extents();

	c -= aabb.center();

	Vec3f v(std::abs(w.x), std::abs(w.y), std::abs(w.z));

	if (std::abs(c.x) > v.x + h.x || std::abs(c.y) > v.y + h.y || std::abs(c.z) > v.z + h.z)
		return false;

	if (std::abs(c.y * w.z - c.z * w.y) > h.y * v.z + h.z * v.y ||
		std::abs(c.x * w.z - c.z * w.x) > h.x * v.z + h.z * v.x ||
		std::abs(c.x * w.y - c.y * w.x) > h.x * v.y + h.y * v.x)
		return false;

	return true;
}

float TriangleMeshShape::sweep_intersect_triangle_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int a, const Vec3f &target)
{
	const int start_element = shape1->nodes[a].element_index;

	Vec3f p[3] =
	{
		shape1->vertices[shape1->elements[start_element]],
		shape1->vertices[shape1->elements[start_element + 1]],
		shape1->vertices[shape1->elements[start_element + 2]]
	};

	Vec3f c = shape2->center;
	Vec3f e = target;
	float r = shape2->radius;

	// Dynamic intersection test between a ray and the minkowski sum of the sphere and polygon:

	Vec3f n = Vec3f::normalize(Vec3f::cross(p[1] - p[0], p[2] - p[0]));
	Vec4f plane(n, -Vec3f::dot(n, p[0]));

	// Step 1: Plane intersect test

	float sc = Vec4f::dot4(plane, Vec4f(c, 1.0f));
	float se = Vec4f::dot4(plane, Vec4f(e, 1.0f));
	bool same_side = sc * se > 0.0f;

	if (same_side && std::abs(sc) > r && std::abs(se) > r)
		return 1.0f;

	// Step 1a: Check if point is in polygon (using crossing ray test in 2d)
	{
		float t = (sc - r) / (sc - se);

		Vec3f vt = c + t * (e - c);

		Vec3f u0 = p[1] - p[0];
		Vec3f u1 = p[2] - p[0];

		Vec2f v_2d[3] =
		{
			Vec2f(0.0f, 0.0f),
			Vec2f(Vec3f::dot(u0, u0), 0.0f),
			Vec2f(0.0f, Vec3f::dot(u1, u1))
		};

		Vec2f point(Vec3f::dot(u0, vt), Vec3f::dot(u1, vt));

		bool inside = false;
		Vec2f e0 = v_2d[2];
		bool y0 = e0.y >= point.y;
		for (int i = 0; i < 3; i++)
		{
			Vec2f e1 = v_2d[i];
			bool y1 = e1.y >= point.y;

			if (y0 != y1 && ((e1.y - point.y) * (e0.x - e1.x) >= (e1.x - point.x) * (e0.y - e1.y)) == y1)
				inside = !inside;

			y0 = y1;
			e0 = e1;
		}

		if (inside)
			return t;
	}

	// Step 2: Edge intersect test

	Vec3f ke[3] =
	{
		p[1] - p[0],
		p[2] - p[1],
		p[0] - p[2],
	};

	Vec3f kg[3] =
	{
		p[0] - c,
		p[1] - c,
		p[2] - c,
	};

	Vec3f ks = e - c;

	float kgg[3];
	float kgs[3];
	float kss[3];

	for (int i = 0; i < 3; i++)
	{
		float kee = Vec3f::dot(ke[i], ke[i]);
		float keg = Vec3f::dot(ke[i], kg[i]);
		float kes = Vec3f::dot(ke[i], ks);
		kgg[i] = Vec3f::dot(kg[i], kg[i]);
		kgs[i] = Vec3f::dot(kg[i], ks);
		kss[i] = Vec3f::dot(ks, ks);

		float aa = kee * kss[i] - kes * kes;
		float bb = 2 * (keg * kes - kee * kgs[i]);
		float cc = kee * (kgg[i] - r * r) - keg * keg;

		float sign = (bb >= 0.0f) ? 1.0f : -1.0f;
		float q = -0.5f * (bb + sign * std::sqrt(bb * bb - 4 * aa * cc));
		float t0 = q / aa;
		float t1 = cc / q;

		float t;
		if (t0 < 0.0f || t0 > 1.0f)
			t = t1;
		else if (t1 < 0.0f || t1 > 1.0f)
			t = t0;
		else
			t = std::min(t0, t1);

		if (t >= 0.0f && t <= 1.0f)
		{
			Vec3f ct = c + t * ks;
			float d = Vec3f::dot(ct - p[i], ke[i]);
			if (d >= 0.0f && d <= kee)
				return t;
		}
	}

	// Step 3: Point intersect test

	for (int i = 0; i < 3; i++)
	{
		float aa = kss[i];
		float bb = -2.0f * kgs[i];
		float cc = kgg[i] - r * r;

		float sign = (bb >= 0.0f) ? 1.0f : -1.0f;
		float q = -0.5f * (bb + sign * std::sqrt(bb * bb - 4 * aa * cc));
		float t0 = q / aa;
		float t1 = cc / q;

		float t;
		if (t0 < 0.0f || t0 > 1.0f)
			t = t1;
		else if (t1 < 0.0f || t1 > 1.0f)
			t = t0;
		else
			t = std::min(t0, t1);

		if (t >= 0.0f && t <= 1.0f)
			return t;
	}

	return 1.0f;
}

bool TriangleMeshShape::overlap_bv(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b)
{
	return IntersectionTest::aabb(shape1->nodes[a].aabb, shape2->nodes[b].aabb) == IntersectionTest::overlap;
}

bool TriangleMeshShape::overlap_bv_triangle(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b)
{
	return false;
}

bool TriangleMeshShape::overlap_bv_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int a)
{
	return IntersectionTest::sphere_aabb(shape2->center, shape2->radius, shape1->nodes[a].aabb) == IntersectionTest::overlap;
}

bool TriangleMeshShape::overlap_triangle_triangle(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b)
{
	return false;
}

bool TriangleMeshShape::overlap_triangle_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int shape1_node_index)
{
	// http://realtimecollisiondetection.net/blog/?p=103

	int element_index = shape1->nodes[shape1_node_index].element_index;

	Vec3f P = shape2->center;
	Vec3f A = shape1->vertices[shape1->elements[element_index]] - P;
	Vec3f B = shape1->vertices[shape1->elements[element_index + 1]] - P;
	Vec3f C = shape1->vertices[shape1->elements[element_index + 2]] - P;
	float r = shape2->radius;
	float rr = r * r;

	// Testing if sphere lies outside the triangle plane
	Vec3f V = Vec3f::cross(B - A, C - A);
	float d = Vec3f::dot(A, V);
	float e = Vec3f::dot(V, V);
	bool sep1 = d * d > rr * e;

	// Testing if sphere lies outside a triangle vertex
	float aa = Vec3f::dot(A, A);
	float ab = Vec3f::dot(A, B);
	float ac = Vec3f::dot(A, C);
	float bb = Vec3f::dot(B, B);
	float bc = Vec3f::dot(B, C);
	float cc = Vec3f::dot(C, C);
	bool sep2 = (aa > rr) && (ab > aa) && (ac > aa);
	bool sep3 = (bb > rr) && (ab > bb) && (bc > bb);
	bool sep4 = (cc > rr) && (ac > cc) && (bc > cc);

	// Testing if sphere lies outside a triangle edge
	Vec3f AB = B - A;
	Vec3f BC = C - B;
	Vec3f CA = A - C;
	float d1 = ab - aa;
	float d2 = bc - bb;
	float d3 = ac - cc;
	float e1 = Vec3f::dot(AB, AB);
	float e2 = Vec3f::dot(BC, BC);
	float e3 = Vec3f::dot(CA, CA);
	Vec3f Q1 = A * e1 - d1 * AB;
	Vec3f Q2 = B * e2 - d2 * BC;
	Vec3f Q3 = C * e3 - d3 * CA;
	Vec3f QC = C * e1 - Q1;
	Vec3f QA = A * e2 - Q2;
	Vec3f QB = B * e3 - Q3;
	bool sep5 = (Vec3f::dot(Q1, Q1) > rr * e1 * e1) && (Vec3f::dot(Q1, QC) > 0.0f);
	bool sep6 = (Vec3f::dot(Q2, Q2) > rr * e2 * e2) && (Vec3f::dot(Q2, QA) > 0.0f);
	bool sep7 = (Vec3f::dot(Q3, Q3) > rr * e3 * e3) && (Vec3f::dot(Q3, QB) > 0.0f);

	bool separated = sep1 || sep2 || sep3 || sep4 || sep5 || sep6 || sep7;
	return (!separated);
}

bool TriangleMeshShape::is_leaf(int node_index)
{
	return nodes[node_index].element_index != -1;
}

float TriangleMeshShape::volume(int node_index)
{
	Vec3f extents = nodes[node_index].aabb.extents();
	return extents.x * extents.y * extents.z;
}

int TriangleMeshShape::get_min_depth()
{
	std::function<int(int, int)> visit;
	visit = [&](int level, int node_index) -> int {
		const Node &node = nodes[node_index];
		if (node.element_index == -1)
			return std::min(visit(level + 1, node.left), visit(level + 1, node.right));
		else
			return level;
	};
	return visit(1, root);
}

int TriangleMeshShape::get_max_depth()
{
	std::function<int(int, int)> visit;
	visit = [&](int level, int node_index) -> int {
		const Node &node = nodes[node_index];
		if (node.element_index == -1)
			return std::max(visit(level + 1, node.left), visit(level + 1, node.right));
		else
			return level;
	};
	return visit(1, root);
}

float TriangleMeshShape::get_average_depth()
{
	std::function<float(int, int)> visit;
	visit = [&](int level, int node_index) -> float {
		const Node &node = nodes[node_index];
		if (node.element_index == -1)
			return visit(level + 1, node.left) + visit(level + 1, node.right);
		else
			return (float)level;
	};
	float depth_sum = visit(1, root);
	int leaf_count = (num_elements / 3);
	return depth_sum / leaf_count;
}

float TriangleMeshShape::get_balanced_depth()
{
	return std::log2((float)(num_elements / 3));
}

int TriangleMeshShape::subdivide(int *triangles, int num_triangles, const Vec3f *centroids, int *work_buffer)
{
	if (num_triangles == 0)
		return -1;

	// Find bounding box and median of the triangle centroids
	Vec3f median;
	Vec3f aabb_min, aabb_max;
	aabb_min = vertices[elements[triangles[0] * 3]];
	aabb_max = aabb_min;
	for (int i = 0; i < num_triangles; i++)
	{
		int element_index = triangles[i] * 3;
		for (int j = 0; j < 3; j++)
		{
			const Vec3f &vertex = vertices[elements[element_index + j]];

			aabb_min.x = std::min(aabb_min.x, vertex.x);
			aabb_min.y = std::min(aabb_min.y, vertex.y);
			aabb_min.z = std::min(aabb_min.z, vertex.z);

			aabb_max.x = std::max(aabb_max.x, vertex.x);
			aabb_max.y = std::max(aabb_max.y, vertex.y);
			aabb_max.z = std::max(aabb_max.z, vertex.z);
		}

		median += centroids[triangles[i]];
	}
	median /= (float)num_triangles;

	if (num_triangles == 1) // Leaf node
	{
		nodes.push_back(Node(aabb_min, aabb_max, triangles[0] * 3));
		return nodes.size() - 1;
	}

	// Find the longest axis
	float axis_lengths[3] =
	{
		aabb_max.x - aabb_min.x,
		aabb_max.y - aabb_min.z,
		aabb_max.z - aabb_min.z
	};

	int axis_order[3] = { 0, 1, 2 };
	std::sort(axis_order, axis_order + 3, [&](int a, int b) { return axis_lengths[a] > axis_lengths[b]; });

	// Try split at longest axis, then if that fails the next longest, and then the remaining one
	int left_count, right_count;
	Vec3f axis;
	for (int attempt = 0; attempt < 3; attempt++)
	{
		// Find the split plane for axis
		switch (axis_order[attempt])
		{
		default:
		case 0: axis = Vec3f(1.0f, 0.0f, 0.0f); break;
		case 1: axis = Vec3f(0.0f, 1.0f, 0.0f); break;
		case 2: axis = Vec3f(0.0f, 0.0f, 1.0f); break;
		}
		Vec4f plane(axis, -Vec3f::dot(median, axis));

		// Split triangles into two
		left_count = 0;
		right_count = 0;
		for (int i = 0; i < num_triangles; i++)
		{
			int triangle = triangles[i];
			int element_index = triangle * 3;

			float side = Vec4f::dot4(Vec4f(centroids[triangles[i]], 1.0f), plane);
			if (side >= 0.0f)
			{
				work_buffer[left_count] = triangle;
				left_count++;
			}
			else
			{
				work_buffer[num_triangles + right_count] = triangle;
				right_count++;
			}
		}

		if (left_count != 0 && right_count != 0)
			break;
	}

	// Check if something went wrong when splitting and do a random split instead
	if (left_count == 0 || right_count == 0) 
	{
		left_count = num_triangles / 2;
		right_count = num_triangles - left_count;
	}

	// Move result back into triangles list:
	for (int i = 0; i < left_count; i++)
		triangles[i] = work_buffer[i];
	for (int i = 0; i < right_count; i++)
		triangles[i + left_count] = work_buffer[num_triangles + i];

	// Create child nodes:
	int left_index = -1;
	int right_index = -1;
	if (left_count > 0)
		left_index = subdivide(triangles, left_count, centroids, work_buffer);
	if (right_count > 0)
		right_index = subdivide(triangles + left_count, right_count, centroids, work_buffer);

	nodes.push_back(Node(aabb_min, aabb_max, left_index, right_index));
	return nodes.size() - 1;
}
