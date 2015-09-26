
#pragma once

class ShaderSetup
{
public:
	static uicore::ProgramObject compile(uicore::GraphicContext &gc, std::string shader_path, const std::string &vertex, const std::string &fragment, const std::string &defines = std::string());
	static void link(uicore::ProgramObject &program, const std::string &program_name);
};
