#include "Editor/Editor.hpp"

class BonfireProject : public Engine
{
public:
    BonfireProject() : Engine("Bonfire")
    {
        GetInstance().SetEngineRunState(false);
        GetInstance().SetEditorRunState(true);
        PushLayer(std::make_shared<Editor>("Data/editorconfig.bonfire"));
    }
    ~BonfireProject() override = default;
};

int main(int argc, char** argv)
{
    BonfireProject* project = new BonfireProject();
    project->Run();
    delete project;

    return 0;
}