#include "BonfireEngine.hpp"

class BonfireProject : public Bonfire::Project
{
public:
	BonfireProject() : Project("Bonfire")
	{
		
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