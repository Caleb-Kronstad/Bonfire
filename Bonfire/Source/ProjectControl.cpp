#include "BonfireEngine.hpp"

#include "Player.hpp"
#include "Enemy.hpp"

class BonfireProject : public Project
{
public:
	BonfireProject() : Project("Bonfire")
	{
		PushLayer(std::make_shared<Player>());
		PushLayer(std::make_shared<StoneGolem>());
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