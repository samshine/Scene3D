
#pragma once

class SphereShape;

class TriangleMeshShape
{
public:
	TriangleMeshShape(const uicore::Vec3f *vertices, int num_vertices, const unsigned int *elements, int num_elements);

	int get_min_depth();
	int get_max_depth();
	float get_average_depth();
	float get_balanced_depth();

	static float sweep(TriangleMeshShape *shape1, SphereShape *shape2, const uicore::Vec3f &target);

	static bool find_any_hit(TriangleMeshShape *shape1, TriangleMeshShape *shape2);
	static bool find_any_hit(TriangleMeshShape *shape1, SphereShape *shape2);
	static bool find_any_hit(TriangleMeshShape *shape, const uicore::Vec3f &ray_start, const uicore::Vec3f &ray_end);

	struct Node
	{
		Node() : left(-1), right(-1), element_index(-1) { }
		Node(const uicore::Vec3f &aabb_min, const uicore::Vec3f &aabb_max, int element_index) : aabb(aabb_min, aabb_max), left(-1), right(-1), element_index(element_index) { }
		Node(const uicore::Vec3f &aabb_min, const uicore::Vec3f &aabb_max, int left, int right) : aabb(aabb_min, aabb_max), left(left), right(right), element_index(-1) { }

		uicore::AxisAlignedBoundingBox aabb;
		int left;
		int right;
		int element_index;
	};

	const uicore::Vec3f *vertices;
	const int num_vertices;
	const unsigned int *elements;
	int num_elements;

	std::vector<Node> nodes;
	int root;

private:
	static float sweep(TriangleMeshShape *shape1, SphereShape *shape2, int a, const uicore::Vec3f &target);

	static bool find_any_hit(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b);
	static bool find_any_hit(TriangleMeshShape *shape1, SphereShape *shape2, int a);
	static bool find_any_hit(TriangleMeshShape *shape1, const uicore::Vec3f &ray_start, const uicore::Vec3f &ray_end, int a);

	inline static bool overlap_bv_ray(TriangleMeshShape *shape, const uicore::Vec3f &ray_start, const uicore::Vec3f &ray_end, int a);
	inline static float intersect_triangle_ray(TriangleMeshShape *shape, const uicore::Vec3f &ray_start, const uicore::Vec3f &ray_end, int a);

	inline static bool sweep_overlap_bv_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int a, const uicore::Vec3f &target);
	inline static float sweep_intersect_triangle_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int a, const uicore::Vec3f &target);

	inline static bool overlap_bv(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b);
	inline static bool overlap_bv_triangle(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b);
	inline static bool overlap_bv_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int a);
	inline static bool overlap_triangle_triangle(TriangleMeshShape *shape1, TriangleMeshShape *shape2, int a, int b);
	inline static bool overlap_triangle_sphere(TriangleMeshShape *shape1, SphereShape *shape2, int a);

	inline bool is_leaf(int node_index);
	inline float volume(int node_index);

	int subdivide(int *triangles, int num_triangles, const uicore::Vec3f *centroids, int *work_buffer);
};
