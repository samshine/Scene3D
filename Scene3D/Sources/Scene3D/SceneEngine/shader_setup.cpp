
#include "precomp.h"
#include "shader_setup.h"

using namespace uicore;

ProgramObjectPtr ShaderSetup::compile(const GraphicContextPtr &gc, std::string shader_path, const std::string &vertex, const std::string &fragment, const std::string &defines)
{
	std::string prefix;
	if (gc->shader_language() == shader_glsl)
		prefix += "#version 330\r\n";
	std::vector<std::string> define_list = Text::split(defines, " ");
	for (size_t i = 0; i < define_list.size(); i++)
		prefix += string_format("#define %1\r\n", define_list[i]);
	prefix += "#line 0\r\n";

	auto program = ProgramObject::create(gc);

	if (!vertex.empty())
	{
		auto vertex_shader = ShaderObject::create(gc, ShaderType::vertex, prefix + File::read_all_text(PathHelp::combine(shader_path, vertex)));
		vertex_shader->compile();
		program->attach(vertex_shader);
	}

	if (!fragment.empty())
	{
		auto fragment_shader = ShaderObject::create(gc, ShaderType::fragment, prefix + File::read_all_text(PathHelp::combine(shader_path, fragment)));
		fragment_shader->compile();
		program->attach(fragment_shader);
	}

	return program;
}

void ShaderSetup::link(ProgramObjectPtr &program, const std::string &program_name)
{
	if (!program->try_link())
		throw Exception(string_format("Failed to link %1: %2", program_name, program->get_info_log()));
}
