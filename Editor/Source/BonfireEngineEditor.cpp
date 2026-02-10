#include "Editor/Editor.hpp"

class BonfireEngineEditor : public Engine
{
public:
    BonfireEngineEditor() : Engine("Bonfire")
    {
        Engine::GetInstance().SetProjectRunState(false);
        Engine::GetInstance().SetEditorRunState(true);
        PushLayer(std::make_shared<Editor>("Data/editorconfig.bonfire"));
    }
    ~BonfireEngineEditor() override = default;
};

int main(int argc, char** argv)
{
    BonfireEngineEditor* engine = new BonfireEngineEditor();
    engine->Run();
    delete engine;

    return 0;
}