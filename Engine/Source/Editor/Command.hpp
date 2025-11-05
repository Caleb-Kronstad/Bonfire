#pragma once

namespace Bonfire
{
    class Command
    {
    public:
        virtual ~Command() = default;

        virtual void Execute() = 0;
        virtual void Undo() = 0;
        virtual std::string GetDescription() const = 0;
        bool executed = false;
    };

    class CommandHistory
    {
    public:
        CommandHistory(uint8_t max_history = 64);
        ~CommandHistory() = default;

        void ExecuteCommand(std::shared_ptr<Command> command);
        bool Undo();
        bool Redo();
        void Clear();

        bool CanUndo() const { return current_index > 0; }
        bool CanRedo() const { return current_index < command_history.size(); }
        uint8_t GetHistorySize() const { return command_history.size(); }
        uint8_t GetCurrentIndex() const { return current_index; }

        std::string GetUndoDescription() const;
        std::string GetRedoDescription() const;

    private:
        std::vector<std::shared_ptr<Command>> command_history;
        uint8_t current_index = 0;
        uint8_t max_history;

        void TrimHistory();
    };
}
