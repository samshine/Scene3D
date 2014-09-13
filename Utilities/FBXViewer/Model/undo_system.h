
#pragma once

#include <vector>
#include <memory>

class ModelCommand;

class UndoSystem
{
public:
	bool can_undo() const;
	bool can_redo() const;

	void undo();
	void redo();

	void execute(const std::shared_ptr<ModelCommand> &command);

	template<typename Command, typename... Args>
	void execute(Args&&... args) { execute(std::make_shared<Command>(args...)); }

private:
	std::vector<std::shared_ptr<ModelCommand>> undo_buffer;
	std::vector<std::shared_ptr<ModelCommand>> redo_buffer;
};
