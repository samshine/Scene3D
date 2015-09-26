
#include "precomp.h"
#include "shader_setup.h"

using namespace uicore;

ProgramObject ShaderSetup::compile(GraphicContext &gc, std::string shader_path, const std::string &vertex, const std::string &fragment, const std::string &defines)
{
	std::string prefix;
	if (gc.get_shader_language() == shader_glsl)
		prefix += "#version 330\r\n";
	std::vector<std::string> define_list = StringHelp::split_text(defines, " ");
	for (size_t i = 0; i < define_list.size(); i++)
		prefix += string_format("#define %1\r\n", define_list[i]);
	prefix += "#line 0\r\n";

	ProgramObject program(gc);

	if (!vertex.empty())
	{
		ShaderObject vertex_shader(gc, shadertype_vertex, prefix + File::read_text(PathHelp::combine(shader_path, vertex)));
		if (!vertex_shader.compile())
			throw Exception(vertex_shader.get_info_log());
		program.attach(vertex_shader);
	}

	if (!fragment.empty())
	{
		ShaderObject fragment_shader(gc, shadertype_fragment, prefix + File::read_text(PathHelp::combine(shader_path, fragment)));
		if (!fragment_shader.compile())
			throw Exception(fragment_shader.get_info_log());
		program.attach(fragment_shader);
	}

	return program;
}

void ShaderSetup::link(ProgramObject &program, const std::string &program_name)
{
	if (!program.link())
		throw Exception(string_format("Failed to link %1: %2", program.get_info_log()));
}
