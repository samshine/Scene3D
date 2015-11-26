
#pragma once

#include <string>
#include <functional>
#include <memory>

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

class AssetCompiler
{
public:
	static std::shared_ptr<AssetCompiler> create(const std::string &asset_directory, const std::string &build_directory, const std::function<void(const CompilerMessage&)> &log);

	virtual void clean() = 0;
	virtual void build() = 0;
	virtual void cancel() = 0;
};

typedef std::shared_ptr<AssetCompiler> AssetCompilerPtr;
