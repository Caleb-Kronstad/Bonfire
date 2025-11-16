#include "BonfireEngine.hpp"

#include "Player.hpp"
#include "StoneGolem.hpp"
#include "NPCManager.hpp"

class BonfireProject : public Project
{
public:
	BonfireProject() : Project("Bonfire")
	{
		PushLayer(std::make_shared<Player>());
		PushLayer(std::make_shared<NPCManager>());
		PushLayer(std::make_shared<StoneGolem>("StoneGolem", "Player", "StoneGolemRock"));
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