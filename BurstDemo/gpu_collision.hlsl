
#define FLT_EPSILON 0.00001

struct Node
{
	float3 aabb_min;
	float3 aabb_max;
	int left;
	int right;
	int element_index;
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

bool find_any_hit(float3 ray_start, float3 ray_end);
bool find_any_hit(float3 ray_start, float3 ray_end, int a);
bool is_leaf(int node_index);
bool overlap_bv_ray(float3 ray_start, float3 ray_end, int a);
float intersect_triangle_ray(float3 ray_start, float3 ray_end, int a);

[numthreads(1, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID)
{
	uint width, height;
	output_image.GetDimensions(width, height);
	if (threadId.x >= width || threadId.y >= height)
		return;

	float x = threadId.x;
	float y = threadId.y;

	float4 color = float4(x / width, y / height, 0, 1);

	float3 ray_from = float3(x - width * 0.5, y - height * 0.5, 0);
	float3 ray_to = ray_from + float3(0, 0, 10);

	if (find_any_hit(ray_from, ray_to))
	{
		color.z = 1;
	}

	output_image[threadId.xy] = color;
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
	Node node = nodes[a];

	float3 c = (ray_start + ray_end) * 0.5f;
	float3 w = ray_end - c;
	float3 h = (node.aabb_max - node.aabb_min) * 0.5f; //aabb.extents();

	c -= (node.aabb_max + node.aabb_min) * 0.5f; // aabb.center();

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
