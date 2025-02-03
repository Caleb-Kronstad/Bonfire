#include "Engine.h"

#include "TestLayer.h"

class ProjectInstance : public Project
{
public:
	ProjectInstance() : Project()
	{
		PushLayer(new TestLayer());
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