#include "Editor/Editor.hpp"

class DemoScript : public Bonfire::Script
{
public:
    void Attach() override
    {
        Log::Info("Demo script attached");
        
        test_entity = EntityGetByName("Test");
    }
    void Update(float delta_time) override
    {
        if (!test)
        {
            test = true;
            EntitySetPosition(test_entity, glm::vec3(0.0f, 100.0f, 0.0f));
        }
    }
    void Interface() override
    {
        
    }
    void Input(Bonfire::Input& input) override
    {
        
    }
    void Detach() override
    {
        Log::Info("Demo script detached");
    }
    
private:
    uint32_t test_entity = 0;
    bool test = false;
};

class BonfireEngineEditor : public Engine
{
public:
    BonfireEngineEditor() : Engine("Bonfire")
    {
        Instance().SetProjectRunState(false);
        Instance().SetEditorRunState(true);
        GetScriptManager().ScriptAdd(std::make_shared<DemoScript>());
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