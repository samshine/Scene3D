
#pragma once

class ShaderSetup
{
public:
	static uicore::ProgramObjectPtr compile(const uicore::GraphicContextPtr &gc, const std::string &program_name, const std::string &vertex_code, const std::string &fragment_code, const std::string &defines = std::string());
	static void link(uicore::ProgramObjectPtr &program, const std::string &program_name);
};
