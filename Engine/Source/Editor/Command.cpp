#include "bonfire_pch.hpp"
#include "Command.hpp"

namespace Bonfire
{
    CommandHistory::CommandHistory(uint8_t max_history)
        : max_history(max_history)
    {
        
    }

    void CommandHistory::ExecuteCommand(std::shared_ptr<Command> command)
    {
        if (current_index < command_history.size())
        {
            command_history.erase(command_history.begin() + current_index, command_history.end());
        }

        if (!command->executed)
        {
            command->Execute();
            command->executed = true;
        }
        
        command_history.push_back(command);
        current_index++;
        TrimHistory();
    }

    bool CommandHistory::Undo()
    {
        if (!CanUndo()) return false;
        current_index--;
        command_history[current_index]->Undo();
        return true;
    }
    bool CommandHistory::Redo()
    {
        if (!CanRedo()) return false;
        command_history[current_index]->Execute();
        current_index++;
        return true;
    }
    void CommandHistory::Clear()
    {
        command_history.clear();
        current_index = 0;
    }

    std::string CommandHistory::GetUndoDescription() const
    {
        if (!CanUndo()) return "";
        return command_history[current_index]->GetDescription();
    }
    std::string CommandHistory::GetRedoDescription() const
    {
        if (!CanRedo()) return "";
        return command_history[current_index]->GetDescription();
    }

    void CommandHistory::TrimHistory()
    {
        if (command_history.size() > max_history)
        {
            uint8_t excess = command_history.size() - max_history;
            command_history.erase(command_history.begin(), command_history.begin() + excess);
            current_index -= excess;
        }
    }
}