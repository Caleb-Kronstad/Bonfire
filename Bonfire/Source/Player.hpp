#pragma once

#include "BonfireEngine.hpp"

using namespace Bonfire;

class Player : public Layer
{
public:
	Player();
	~Player();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const float& delta_time) override;
	void OnInterfaceUpdate() override;
	void OnInput(Input& input) override;

private:
	std::shared_ptr<Entity> player;
	std::shared_ptr<Audio> global_audio;
	float move_speed = 5.0f;
};