
#pragma once

#include <fbxsdk.h>
#include <ClanLib/core.h>
#include <scene3d.h>
#include "vertex_mapping.h"
#include "skinned_bone.h"

namespace clan
{
	class FBXModelImpl;
	class ModelDescAnimation;
	class ModelDescAttachmentPoint;
	class ModelDescParticleEmitter;
	class ModelDesc;

	class FBXModelLoader
	{
	public:
		FBXModelLoader(FBXModelImpl *model, const ModelDesc &desc);

		std::shared_ptr<ModelData> model_data;

	private:
		void import_scene(const std::string &filename);
		void triangulate_scene();
		void bake_geometric_transforms(FbxNode *node = nullptr);

		int find_bone_index(const std::string &name);

		void convert_node(FbxNode *node);
		void convert_mesh(FbxNode *node);
		void convert_camera(FbxNode *node);
		void convert_light(FbxNode *node);

		void convert_polygons(FbxMesh *mesh, VertexMappingVector &vertices, std::vector<VertexMapping *> &elements, const Mat4f &mesh_to_world, const Mat3f &normal_mesh_to_world);
		void convert_skins(FbxNode *node, FbxMesh *mesh, VertexMappingVector &vertices);
		void convert_bones(const ModelDescAnimation &animation);

		ModelDataDrawRange create_draw_range(size_t start_element, size_t num_elements, FbxSurfaceMaterial *material);
		ModelDataTextureMap create_texture_channel(int channel, const char *property_name, FbxSurfaceMaterial *material, float gamma);

		Vec4ub get_color(FbxMesh *mesh, int control_index, int vertex_index);
		Vec3f get_normal(FbxMesh *mesh, int control_index, int vertex_index);
		Vec3f get_tangent(FbxMesh *mesh, int control_index, int vertex_index);
		Vec3f get_bitangent(FbxMesh *mesh, int control_index, int vertex_index);
		Vec2f get_uv(FbxMesh *mesh, int control_index, int vertex_index, int uv_channel_index);

		static Vec2f to_vec2f(const FbxVector2 &v);
		static Vec3f to_vec3f(const FbxDouble3 &d);
		static Vec4f to_vec4f(const FbxVector4 &v);
		static Vec4f to_vec4f(const FbxColor &c);
		static Vec4ub to_vec4ub(const FbxColor &c);
		static Mat4f to_mat4f(const FbxAMatrix &m);
		static Quaternionf to_quaternionf(const FbxQuaternion &q);

		const ModelDesc &model_desc;
		FBXModelImpl *model;
		std::vector<SkinnedBone> bones;
	};
}
