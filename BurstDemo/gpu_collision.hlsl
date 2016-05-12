
#define FLT_EPSILON 0.00001

struct Node
{
	float3 aabb_min;
	float3 aabb_max;
	int left;
	int right;
	int element_index;
};

struct SphereShape
{
	float3 center;
	float radius;
};

cbuffer Uniforms
{
	int root;
	int padding1, padding2, padding3; // 16 byte boundary alignment
}

StructuredBuffer<Node> nodes;
StructuredBuffer<float3> vertices;
StructuredBuffer<uint> elements;

RWTexture2D<float4> output_image;

float sweep(SphereShape sphere, float3 target);
float sweep(SphereShape sphere, int node, float3 target);
bool sweep_overlap_bv_sphere(SphereShape sphere, int a, float3 target);
float sweep_intersect_triangle_sphere(SphereShape sphere, int a, float3 target);

bool find_any_hit(float3 ray_start, float3 ray_end);
bool find_any_hit(float3 ray_start, float3 ray_end, int a);
bool is_leaf(int node_index);
bool overlap_bv_ray(float3 ray_start, float3 ray_end, int a);
bool overlap_ray_aabb(float3 ray_start, float3 ray_end, float3 aabb_min, float3 aabb_max);
float intersect_triangle_ray(float3 ray_start, float3 ray_end, int a);

[numthreads(1, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID)
{
	uint width, height;
	output_image.GetDimensions(width, height);
	if (threadId.x >= width || threadId.y >= height)
		return;

	float x = threadId.x;
	float y = height - threadId.y;

	float4 color = float4(x / width, y / height, 0, 1);

	float3 ray_from = float3(x - width * 0.5, y - height * 0.10, -5) * 0.005;
	float3 ray_to = ray_from + float3(0, 0, 10);

	SphereShape sphere;
	sphere.center = ray_from;
	sphere.radius = 0.01;

	float t = sweep(sphere, ray_to);
	color = lerp(float4(1, 1, 1, 1), color, t);

	/*if (find_any_hit(ray_from, ray_to))
	{
		color = float4(1, 1, 1, 1);
	}*/

	output_image[threadId.xy] = color;
}

float sweep(SphereShape sphere, float3 target)
{
	return sweep(sphere, root, target);
}

float sweep(SphereShape sphere, int node, float3 target)
{
	float t = 1;

	int stack[16];
	int stack_pos = 1;
	stack[0] = node;
	while (stack_pos > 0)
	{
		int a = stack[stack_pos - 1];

		if (!sweep_overlap_bv_sphere(sphere, a, target))
		{
			stack_pos--;
		}
		else if (is_leaf(a))
		{
			t = min(sweep_intersect_triangle_sphere(sphere, a, target), t);
			stack_pos--;
		}
		else if (stack_pos == 16)
		{
			stack_pos--; // stack overflow
		}
		else
		{
			stack[stack_pos - 1] = nodes[a].left;
			stack[stack_pos] = nodes[a].right;
			stack_pos++;
		}
	}

	return t;
}

bool sweep_overlap_bv_sphere(SphereShape sphere, int a, float3 target)
{
	// Convert to ray test by expanding the AABB:

	float3 aabb_min = nodes[a].aabb_min - sphere.radius;
	float3 aabb_max = nodes[a].aabb_max + sphere.radius;

	return overlap_ray_aabb(sphere.center, target, aabb_min, aabb_max);
}

float sweep_intersect_triangle_sphere(SphereShape sphere, int a, float3 target)
{
	const int start_element = nodes[a].element_index;

	float3 p[3] =
	{
		vertices[elements[start_element]],
		vertices[elements[start_element + 1]],
		vertices[elements[start_element + 2]]
	};

	float3 c = sphere.center;
	float3 e = target;
	float r = sphere.radius;

	// Dynamic intersection test between a ray and the minkowski sum of the sphere and polygon:

	float3 n = normalize(cross(p[1] - p[0], p[2] - p[0]));
	float4 plane = float4(n, -dot(n, p[0]));

	// Step 1: Plane intersect test

	float sc = dot(plane, float4(c, 1.0f));
	float se = dot(plane, float4(e, 1.0f));
	bool same_side = sc * se > 0.0f;

	if (same_side && abs(sc) > r && abs(se) > r)
		return 1.0f;

	// Step 1a: Check if point is in polygon (using crossing ray test in 2d)
	{
		float t = (sc - r) / (sc - se);

		float3 vt = c + t * (e - c);

		float3 u0 = p[1] - p[0];
		float3 u1 = p[2] - p[0];

		float2 v_2d[3] =
		{
			float2(0.0f, 0.0f),
			float2(dot(u0, u0), 0.0f),
			float2(0.0f, dot(u1, u1))
		};

		float2 point_ = float2(dot(u0, vt), dot(u1, vt));

		bool inside = false;
		float2 e0 = v_2d[2];
		bool y0 = e0.y >= point_.y;
		for (int i = 0; i < 3; i++)
		{
			float2 e1 = v_2d[i];
			bool y1 = e1.y >= point_.y;

			if (y0 != y1 && ((e1.y - point_.y) * (e0.x - e1.x) >= (e1.x - point_.x) * (e0.y - e1.y)) == y1)
				inside = !inside;

			y0 = y1;
			e0 = e1;
		}

		if (inside)
			return t;
	}

	// Step 2: Edge intersect test

	float3 ke[3] =
	{
		p[1] - p[0],
		p[2] - p[1],
		p[0] - p[2],
	};

	float3 kg[3] =
	{
		p[0] - c,
		p[1] - c,
		p[2] - c,
	};

	float3 ks = e - c;

	float kgg[3];
	float kgs[3];
	float kss[3];

	int i;

	for (i = 0; i < 3; i++)
	{
		float kee = dot(ke[i], ke[i]);
		float keg = dot(ke[i], kg[i]);
		float kes = dot(ke[i], ks);
		kgg[i] = dot(kg[i], kg[i]);
		kgs[i] = dot(kg[i], ks);
		kss[i] = dot(ks, ks);

		float aa = kee * kss[i] - kes * kes;
		float bb = 2 * (keg * kes - kee * kgs[i]);
		float cc = kee * (kgg[i] - r * r) - keg * keg;

		float sign = (bb >= 0.0f) ? 1.0f : -1.0f;
		float q = -0.5f * (bb + sign * sqrt(bb * bb - 4 * aa * cc));
		float t0 = q / aa;
		float t1 = cc / q;

		float t;
		if (t0 < 0.0f || t0 > 1.0f)
			t = t1;
		else if (t1 < 0.0f || t1 > 1.0f)
			t = t0;
		else
			t = min(t0, t1);

		if (t >= 0.0f && t <= 1.0f)
		{
			float3 ct = c + t * ks;
			float d = dot(ct - p[i], ke[i]);
			if (d >= 0.0f && d <= kee)
				return t;
		}
	}

	// Step 3: Point intersect test

	for (i = 0; i < 3; i++)
	{
		float aa = kss[i];
		float bb = -2.0f * kgs[i];
		float cc = kgg[i] - r * r;

		float sign = (bb >= 0.0f) ? 1.0f : -1.0f;
		float q = -0.5f * (bb + sign * sqrt(bb * bb - 4 * aa * cc));
		float t0 = q / aa;
		float t1 = cc / q;

		float t;
		if (t0 < 0.0f || t0 > 1.0f)
			t = t1;
		else if (t1 < 0.0f || t1 > 1.0f)
			t = t0;
		else
			t = min(t0, t1);

		if (t >= 0.0f && t <= 1.0f)
			return t;
	}

	return 1.0f;
}

bool find_any_hit(float3 ray_start, float3 ray_end)
{
	return find_any_hit(ray_start, ray_end, root);
}

bool find_any_hit(float3 ray_start, float3 ray_end, int node)
{
	int stack[16];
	int stack_pos = 1;
	stack[0] = node;
	while (stack_pos > 0)
	{
		int a = stack[stack_pos - 1];

		if (!overlap_bv_ray(ray_start, ray_end, a))
		{
			stack_pos--;
		}
		else if (is_leaf(a))
		{
			if (intersect_triangle_ray(ray_start, ray_end, a) < 1.0f)
				return true;
			stack_pos--;
		}
		else if (stack_pos == 16)
		{
			stack_pos--; // stack overflow
		}
		else
		{
			stack[stack_pos - 1] = nodes[a].left;
			stack[stack_pos] = nodes[a].right;
			stack_pos++;
		}
	}
	return false;
}

bool is_leaf(int node_index)
{
	return nodes[node_index].element_index != -1;
}

bool overlap_bv_ray(float3 ray_start, float3 ray_end, int a)
{
	return overlap_ray_aabb(ray_start, ray_end, nodes[a].aabb_min, nodes[a].aabb_max);
}

bool overlap_ray_aabb(float3 ray_start, float3 ray_end, float3 aabb_min, float3 aabb_max)
{
	float3 c = (ray_start + ray_end) * 0.5f;
	float3 w = ray_end - c;
	float3 h = (aabb_max - aabb_min) * 0.5f; //aabb.extents();

	c -= (aabb_max + aabb_min) * 0.5f; // aabb.center();

	float3 v = abs(w);

	if (abs(c.x) > v.x + h.x || abs(c.y) > v.y + h.y || abs(c.z) > v.z + h.z)
		return false; // disjoint;

	if (abs(c.y * w.z - c.z * w.y) > h.y * v.z + h.z * v.y ||
		abs(c.x * w.z - c.z * w.x) > h.x * v.z + h.z * v.x ||
		abs(c.x * w.y - c.y * w.x) > h.x * v.y + h.y * v.x)
		return false; // disjoint;

	return true; // overlap;
}

float intersect_triangle_ray(float3 ray_start, float3 ray_end, int a)
{
	const int start_element = nodes[a].element_index;

	float3 p[3] =
	{
		vertices[elements[start_element]],
		vertices[elements[start_element + 1]],
		vertices[elements[start_element + 2]]
	};

	// Moeller–Trumbore ray-triangle intersection algorithm:

	float3 D = ray_end - ray_start;

	// Find vectors for two edges sharing p[0]
	float3 e1 = p[1] - p[0];
	float3 e2 = p[2] - p[0];

	// Begin calculating determinant - also used to calculate u parameter
	float3 P = cross(D, e2);
	float det = dot(e1, P);

	// If determinant is near zero, ray lies in plane of triangle
	if (det > -FLT_EPSILON && det < FLT_EPSILON)
		return 1.0f;

	float inv_det = 1.0f / det;

	// Calculate distance from p[0] to ray origin
	float3 T = ray_start - p[0];

	// Calculate u parameter and test bound
	float u = dot(T, P) * inv_det;

	// Check if the intersection lies outside of the triangle
	if (u < 0.f || u > 1.f)
		return 1.0f;

	// Prepare to test v parameter
	float3 Q = cross(T, e1);

	// Calculate V parameter and test bound
	float v = dot(D, Q) * inv_det;

	// The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f)
		return 1.0f;

	float t = dot(e2, Q) * inv_det;
	if (t > FLT_EPSILON)
		return t;

	// No hit, no win
	return 1.0f;
}
