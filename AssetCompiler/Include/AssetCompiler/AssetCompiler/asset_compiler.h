
#pragma once

#include <string>
#include <functional>
#include <memory>

namespace uicore
{
	enum class CompilerMessageType
	{
		error,
		warning,
		info
	};

	class CompilerMessage
	{
	public:
		CompilerMessage() { }
		CompilerMessage(CompilerMessageType type, const std::string &text, const std::string &file = std::string(), int line = 0) : type(type), text(text), file(file), line(line) { }

		CompilerMessageType type = CompilerMessageType::info;
		std::string text;
		std::string file;
		int line = 0;
	};

	class AssetCompilerImpl;

	class AssetCompiler
	{
	public:
		AssetCompiler();
		void compile(const std::string &filename, const std::function<void(const CompilerMessage&)> &output);

	private:
		std::shared_ptr<AssetCompilerImpl> impl;
	};
}
