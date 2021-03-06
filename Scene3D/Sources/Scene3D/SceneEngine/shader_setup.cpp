
#include "precomp.h"
#include "shader_setup.h"

using namespace uicore;

ProgramObjectPtr ShaderSetup::compile(const GraphicContextPtr &gc, const std::string &program_name, const std::string &vertex_code, const std::string &fragment_code, const std::string &defines)
{
	std::string prefix;
	if (gc->shader_language() == shader_glsl)
		prefix += "#version 330\r\n";
	std::vector<std::string> define_list = Text::split(defines, " ");
	for (size_t i = 0; i < define_list.size(); i++)
		prefix += string_format("#define %1\r\n", define_list[i]);
	prefix += "#line 0\r\n";

	auto program = ProgramObject::create(gc);

	if (!vertex_code.empty())
	{
		try
		{
			auto vertex_shader = ShaderObject::create(gc, ShaderType::vertex, prefix + vertex_code);
			vertex_shader->compile();
			program->attach(vertex_shader);
		}
		catch (const Exception &e)
		{
			throw Exception(string_format("%1 vertex shader: %2", program_name, e.message));
		}
	}

	if (!fragment_code.empty())
	{
		try
		{
			auto fragment_shader = ShaderObject::create(gc, ShaderType::fragment, prefix + fragment_code);
			fragment_shader->compile();
			program->attach(fragment_shader);
		}
		catch (const Exception &e)
		{
			throw Exception(string_format("%1 fragment shader: %2", program_name, e.message));
		}
	}

	return program;
}

void ShaderSetup::link(ProgramObjectPtr &program, const std::string &program_name)
{
	if (!program->try_link())
		throw Exception(string_format("Failed to link %1: %2", program_name, program->info_log()));
}
