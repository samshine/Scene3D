
#include "precomp.h"
#include "gpu_collision.h"

using namespace uicore;

GPUCollision::GPUCollision(GraphicContextPtr gc) : gc(gc)
{
	auto shader = ShaderObject::create(gc, ShaderType::compute, File::read_all_text("gpu_collision.hlsl"));
	shader->compile();

	program = ProgramObject::create(gc);
	program->attach(shader);
	program->link();
	program->set_uniform_buffer_index("Uniforms", 0);
	program->set_storage_buffer_index("nodes", 0);
	program->set_storage_buffer_index("elements", 1);
	program->set_storage_buffer_index("vertices", 2);
	program->set_uniform1i("output_image", 0);
}

void GPUCollision::set_shape(std::shared_ptr<ModelData> model_data)
{
	set_shape(model_data->meshes.front().vertices.data(), model_data->meshes.front().vertices.size(), model_data->meshes.front().elements.data(), model_data->meshes.front().elements.size());
}

void GPUCollision::set_shape(const Vec3f * vertices, int num_vertices, const unsigned int * elements, int num_elements)
{
	TriangleMeshShape cpu_shape(vertices, num_vertices, elements, num_elements);

	UniformsBlock cpu_uniforms;
	cpu_uniforms.root = cpu_shape.root;

	gpu_nodes = StorageBuffer::create(gc, cpu_shape.nodes.data(), sizeof(TriangleMeshShape::Node) * cpu_shape.nodes.size(), sizeof(TriangleMeshShape::Node));
	gpu_elements = StorageBuffer::create(gc, cpu_shape.elements, sizeof(unsigned int) * cpu_shape.num_elements, sizeof(unsigned int));
	gpu_vertices = StorageBuffer::create(gc, cpu_shape.vertices, sizeof(Vec3f) * cpu_shape.num_vertices, sizeof(Vec3f));

	gpu_uniforms = UniformBuffer::create(gc, &cpu_uniforms, sizeof(UniformsBlock));

	gpu_output_image = Texture2D::create(gc, 256, 256, tf_rgba8);
}

void GPUCollision::update()
{
	gc->set_program_object(program);
	gc->set_uniform_buffer(0, gpu_uniforms);
	gc->set_storage_buffer(0, gpu_nodes);
	gc->set_storage_buffer(1, gpu_elements);
	gc->set_storage_buffer(2, gpu_vertices);
	gc->set_image_texture(0, gpu_output_image);

	gc->dispatch(256, 256);

	gc->set_uniform_buffer(0, nullptr);
	gc->set_storage_buffer(0, nullptr);
	gc->set_storage_buffer(1, nullptr);
	gc->set_storage_buffer(2, nullptr);
	gc->set_image_texture(0, nullptr);
	gc->set_program_object(nullptr);
}
