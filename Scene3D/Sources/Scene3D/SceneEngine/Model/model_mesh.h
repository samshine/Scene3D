
#pragma once

#include "model_mesh_buffers.h"
#include "Scene3D/SceneEngine/resource.h"

class SceneEngineImpl;
class ModelData;

class ModelMesh
{
public:
	ModelMesh(SceneEngineImpl *engine, const uicore::GraphicContextPtr &gc, std::shared_ptr<ModelData> model_data);

	int vectors_per_instance() const;

	static const int vectors_per_bone = 3;
	static const int instance_base_vectors = 16;
	static const int vectors_per_material = 14;

	std::shared_ptr<ModelData> model_data;
	std::vector<ModelMeshBuffers> mesh_buffers;
	std::vector<Resource<uicore::TexturePtr> > textures;

private:
	template<typename Type>
	uicore::VertexArrayVector<Type> upload_vector(const uicore::GraphicContextPtr &gc, const uicore::PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec);

	template<typename Type>
	uicore::VertexArrayVector<Type> upload_vector(const uicore::GraphicContextPtr &gc, const uicore::PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec, bool normalize);
};
