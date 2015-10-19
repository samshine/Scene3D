
#pragma once

#include <physics3d.h>
#include <Collision3D/triangle_mesh_shape.h>

template<typename T>
class LightmapBuffer
{
public:
	LightmapBuffer(int width, int height) : _width(width), _height(height)
	{
		_values.resize(_width * _height);
	}

	int width() const { return _width; }
	int height() const { return _height; }

	T &at(int x, int y) { return _values.at(x + y * _width); }
	const T &at(int x, int y) const { return _values.at(x + y * _width); }

	const T *data() const { return _values.data(); }

private:
	int _width;
	int _height;
	std::vector<T> _values;
};

class LightmapBuffers
{
public:
	LightmapBuffers(int width, int height) : light(width, height), position(width, height), normal(width, height) { }

	LightmapBuffer<uicore::Vec3f> light;
	LightmapBuffer<uicore::Vec3f> position;
	LightmapBuffer<uicore::Vec3f> normal;
};

class EdgeFunction
{
public:
	EdgeFunction(const uicore::Vec2f &v0, const uicore::Vec2f &v1, const uicore::Vec2f &origin, float sign = 1.0f) : value_at_origin(orient_2d(v0, v1, origin) * sign), step_x((v0.y - v1.y) * sign), step_y((v1.x - v0.x) * sign)
	{
		reset();
	}

	void reset()
	{
		value_row = value_at_origin;
		value = value_row;
	}

	void next_row()
	{
		value_row += step_y;
		value = value_row;
	}

	void next_x()
	{
		value += step_x;
	}

	static float orient_2d(const uicore::Vec2f &a, const uicore::Vec2f &b, const uicore::Vec2f &c)
	{
		return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
	}

	const float value_at_origin;
	const float step_x;
	const float step_y;
	float value_row;
	float value;
};

class LightmapTexture
{
public:
	void generate(const std::shared_ptr<ModelData> &model_data);

private:
	void create_light_maps();
	void generate_face(int target_texture, const uicore::Vec2f *uv, const uicore::Vec3f *vertices, const uicore::Vec3f *normals);
	void generate_face_fragment(std::shared_ptr<LightmapBuffers> &lightmap, int x, int y, int target_texture, float a, float b, float c, const uicore::Vec3f *vertices, const uicore::Vec3f *normals);

	void create_collision_mesh();

	uicore::Vec3f uniform_sample_hemisphere(float random1, float random2);
	uicore::Vec3f cosine_sample_hemisphere(float random1, float random2);

	void raytrace();
	void shooting_rays();
	void ambient_occulusion(std::shared_ptr<LightmapBuffers> &lightmap, int x, int y);
	void raytrace_visible_lights(std::shared_ptr<LightmapBuffers> &lightmap, int x, int y);

	void outline_extend();
	void blur();

	void save_lightmaps();
	
	std::map<int, std::shared_ptr<LightmapBuffers>> lightmaps;

	std::shared_ptr<ModelData> model_data;

	std::shared_ptr<TriangleMeshShape> triangle_mesh;
	std::vector<unsigned int> triangle_elements;

	Physics3DWorldPtr world = Physics3DWorld::create();
	Physics3DObject model_collision;

	const float margin = 0.01f;
};
