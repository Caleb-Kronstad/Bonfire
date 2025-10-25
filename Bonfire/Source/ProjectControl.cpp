#include "BonfireEngine.hpp"

#include "TestLayer.hpp"

class BonfireProject : public Project
{
public:
	BonfireProject()
	{
		PushLayer(std::make_shared<TestLayer>());
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