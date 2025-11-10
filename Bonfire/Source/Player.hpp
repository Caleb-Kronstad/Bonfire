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
	std::shared_ptr<Entity> arm;
	std::shared_ptr<Entity> sword;
	std::shared_ptr<Audio> global_audio;

	glm::vec3 camera_offset = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 arm_offset = glm::vec3(1.5f, -1.0f, 1.0f);
	glm::vec3 sword_offset = glm::vec3(-1.0f, 0.1f, 0.0f);
	
	float move_speed = 5.0f;
};