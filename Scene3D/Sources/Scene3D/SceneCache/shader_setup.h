
#pragma once

class ShaderSetup
{
public:
	static uicore::ProgramObjectPtr compile(uicore::GraphicContext &gc, std::string shader_path, const std::string &vertex, const std::string &fragment, const std::string &defines = std::string());
	static void link(uicore::ProgramObjectPtr &program, const std::string &program_name);
};
