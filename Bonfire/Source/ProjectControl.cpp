#include "BonfireEngine.hpp"

#include "MainLayer.hpp"

class BonfireProject : public Project
{
public:
	BonfireProject() : Project("Bonfire")
	{
		PushLayer(std::make_shared<MainLayer>());
	}

	~BonfireProject()
	{
		
	}

private:

};

int main(int argc, char** argv)
{
	auto project = new BonfireProject();
	project->Run();
	delete project;

	return 0;
}