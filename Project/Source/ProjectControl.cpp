#include "BonfireEngine.hpp"

#include "TestLayer.hpp"

class ProjectInstance : public Project
{
public:
	ProjectInstance()
	{
		PushLayer(std::make_shared<TestLayer>());
	}

	~ProjectInstance()
	{
	}

private:

};

int main(int argc, char** argv)
{
	auto project = new ProjectInstance();
	project->Run();
	delete project;

	return 0;
}