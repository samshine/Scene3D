
#pragma once

class ModelRenderUniforms
{
public:
	unsigned int base_vector_offset = 0;
	unsigned int padding1 = 0;
	unsigned int padding2 = 0;
	unsigned int padding3 = 0;
};

class ModelMaterialUniforms
{
public:
	ModelMaterialUniforms() : material_glossiness(), material_specular_level(), vectors_per_instance(), material_offset() { }
	uicore::Vec4f material_ambient;
	uicore::Vec4f material_diffuse;
	uicore::Vec4f material_specular;
	float material_glossiness;
	float material_specular_level;
	unsigned int vectors_per_instance;
	unsigned int material_offset;
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
	std::vector<uicore::VertexArrayBufferPtr> channels;
	std::vector<uicore::UniformVector<ModelMaterialUniforms> > uniforms;
	uicore::PrimitivesArrayPtr primitives_array;
};
