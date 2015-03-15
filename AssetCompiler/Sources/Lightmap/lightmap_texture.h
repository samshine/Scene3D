
#pragma once

#include <physics3d.h>
#include <Collision3D/triangle_mesh_shape.h>

namespace clan
{
	class LightmapBuffer
	{
	public:
		LightmapBuffer(int width, int height) : _width(width), _height(height)
		{
			_values.resize(_width * _height);
		}

		int width() const { return _width; }
		int height() const { return _height; }

		Vec3f &at(int x, int y) { return _values.at(x + y * _width); }
		const Vec3f &at(int x, int y) const { return _values.at(x + y * _width); }

		const Vec3f *data() const { return _values.data(); }

	private:
		int _width;
		int _height;
		std::vector<Vec3f> _values;
	};

	class LightmapTexture
	{
	public:
		void generate(const std::shared_ptr<ModelData> &model_data);

	private:
		void raytrace_face(int target_texture, const Vec2f *uv, const Vec3f *vertices);
		Vec3f raytrace_face_point(int target_texture, const Vec2f *uv, float px, float py, const Vec3f *vertices, const Vec3f &face_debug_color);
	
		std::map<int, std::shared_ptr<LightmapBuffer>> lightmaps;

		std::shared_ptr<ModelData> model_data;
		std::shared_ptr<TriangleMeshShape> triangle_mesh;
		Physics3DWorld world;
		Physics3DObject model_collision;
	};
}
