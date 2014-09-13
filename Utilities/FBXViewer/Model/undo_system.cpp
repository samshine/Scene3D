
#include "undo_system.h"
#include "Commands/model_command.h"

bool UndoSystem::can_undo() const
{
	return !undo_buffer.empty();
}

bool UndoSystem::can_redo() const
{
	return !redo_buffer.empty();
}

void UndoSystem::undo()
{
	if (can_undo())
	{
		undo_buffer.back()->rollback();
		redo_buffer.push_back(undo_buffer.back());
		undo_buffer.pop_back();
	}
}

void UndoSystem::redo()
{
	if (can_redo())
	{
		redo_buffer.back()->execute();
		undo_buffer.push_back(redo_buffer.back());
		redo_buffer.pop_back();
	}
}

void UndoSystem::execute(const std::shared_ptr<ModelCommand> &command)
{
	command->execute();
	redo_buffer.clear();
	undo_buffer.push_back(command);
}
