
#pragma once

class ModelMaterialUniforms
{
public:
	ModelMaterialUniforms() : material_glossiness(), material_specular_level(), model_index(), vectors_per_instance(), material_offset() { }
	uicore::Vec4f material_ambient;
	uicore::Vec4f material_diffuse;
	uicore::Vec4f material_specular;
	float material_glossiness;
	float material_specular_level;
	unsigned int model_index;
	unsigned int vectors_per_instance;
	unsigned int material_offset;
	unsigned int padding0;
	unsigned int padding1;
	unsigned int padding2;
};

class ModelMeshBuffers
{
public:
	uicore::VertexArrayVector<uicore::Vec3f> vertices;
	uicore::VertexArrayVector<uicore::Vec3f> normals;
	uicore::VertexArrayVector<uicore::Vec3f> bitangents;
	uicore::VertexArrayVector<uicore::Vec3f> tangents;
	uicore::VertexArrayVector<uicore::Vec4ub> bone_weights;
	uicore::VertexArrayVector<uicore::Vec4ub> bone_selectors;
	uicore::VertexArrayVector<uicore::Vec4ub> colors;
	uicore::ElementArrayVector<unsigned int> elements;
	std::vector<uicore::VertexArrayBuffer> channels;
	std::vector<uicore::UniformVector<ModelMaterialUniforms> > uniforms;
	uicore::PrimitivesArray primitives_array;
};
