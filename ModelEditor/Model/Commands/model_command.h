
#pragma once

class ModelCommand
{
public:
	ModelCommand() { }
	virtual ~ModelCommand() { }

	virtual void execute() = 0;
	virtual void rollback() = 0;

private:
	ModelCommand(const ModelCommand &) = delete;
	ModelCommand &operator=(ModelCommand &) = delete;
};
