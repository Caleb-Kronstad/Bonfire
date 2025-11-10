#include "Player.hpp"

Player::Player() : Layer("New Layer")
{

}
Player::~Player()
{

}

void Player::OnAttach()
{
	Project& project = Project::GetInstance();
	Renderer& renderer = Project::GetRenderer();
	PhysicsSystem& physics_system = Project::GetPhysicsSystem();
	AudioSystem& audio_system = Project::GetAudioSystem();
	ScriptSystem& script_system = Project::GetScriptSystem();
	Scene& scene = renderer.GetScene();

	player_stats = PlayerStats();
	weapon_stats = WeaponStats("Greatsword", glm::vec3(-1.0f, 0.1f, 0.0f), 5, 1, 10);

	player = scene.GetEntityByName("Player");
	arm = scene.GetEntityByName("PlayerArm");
	weapon = scene.GetEntityByName(weapon_stats.name);
	
	global_audio = scene.GetEntityByName("Global Audio")->GetComponent<AudioComponent>().audio;
	global_audio->SetSpatialization(false);
	global_audio->Play();
}

void Player::OnDetach()
{
	global_audio->Stop();
}

void Player::OnUpdate(const float& delta_time)
{
	Project& project = Project::GetInstance();
	Window& window = project.GetWindow();
	GLFWwindow* glfw_window = window.GetNativeWindow();
	
	if (!player || !player->HasComponent<CameraComponent>() || !player->HasComponent<PhysicsComponent>())
		return;

	auto& camera_comp = player->GetComponent<CameraComponent>();
	auto& physics_comp = player->GetComponent<PhysicsComponent>();
	Camera* camera = camera_comp.camera.get();
	PhysicsBody* body = physics_comp.physics_body.get();

	glm::quat rotation = glm::quat(glm::vec3(0.0f, glm::radians(camera->yaw), 0.0f));
	body->SetRotation(rotation);
	glm::vec3 current_velocity = body->GetLinearVelocity();

	glm::vec3 forward = camera->GetFrontVector();
	forward.y = 0.0f;
	if (glm::length(forward) > 0.0f)
		forward = glm::normalize(forward);

	glm::vec3 right = camera->GetRightVector();
	right = glm::normalize(right);

	glm::vec3 move_direction(0.0f);

	if (glfwGetKey(glfw_window, InputCode::W) == GLFW_PRESS)
		move_direction += forward;
	if (glfwGetKey(glfw_window, InputCode::S) == GLFW_PRESS)
		move_direction -= forward;
	if (glfwGetKey(glfw_window, InputCode::A) == GLFW_PRESS)
		move_direction -= right;
	if (glfwGetKey(glfw_window, InputCode::D) == GLFW_PRESS)
		move_direction += right;

	if (glm::length(move_direction) > 0.0f)
	{
		move_direction = glm::normalize(move_direction);
		glm::vec3 velocity = move_direction * player_stats.move_speed;
		velocity.y = current_velocity.y;
		body->SetLinearVelocity(velocity);
	}
	else
	{
		body->SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
	}

	glm::vec3 arm_relative_offset = camera->GetRightVector() * arm_offset.x + glm::vec3(0.0f, arm_offset.y, 0.0f) + camera->GetFrontVector() * arm_offset.z;
    arm->position = camera->position + arm_relative_offset;

    float attack_angle = 0.0f;
    if (is_attacking)
    {
            attack_timer += delta_time;
            float progress = attack_timer / attack_duration;

            if (progress >= 1.0f)
            {
                    is_attacking = false;
                    attack_timer = 0.0f;
                    attack_angle = 0.0f;
            }
            else
            {
                    float t = progress * progress * (3.0f - 2.0f * progress);
                    attack_angle = glm::mix(-90.0f, 90.0f, t);
            }
    }

    glm::quat camera_rotation = glm::quat(glm::vec3(0.0f, glm::radians(-camera->yaw + 180.0f), 0.0f));
    glm::quat attack_rotation = glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(attack_angle)));
    glm::quat final_arm_rotation = camera_rotation * attack_rotation;

    arm->rotation = glm::degrees(glm::eulerAngles(final_arm_rotation));

    glm::vec3 rotated_weapon_offset = final_arm_rotation * weapon_stats.offset;
    weapon->position = arm->position + rotated_weapon_offset;

	glm::quat weapon_local_rotation = glm::quat(glm::radians(glm::vec3(270.0f, 270.0f, 0.0f)));
	glm::quat final_weapon_rotation = final_arm_rotation * weapon_local_rotation;
    weapon->rotation = glm::degrees(glm::eulerAngles(final_weapon_rotation));

    player->rotation = glm::vec3(0.0f, glm::radians(camera->yaw), 0.0f);
	
	camera->position = body->GetPosition() + camera_offset;
}
void Player::OnInterfaceUpdate()
{
	
}
void Player::OnInput(Input& input)
{
	Project& project = Project::GetInstance();
	Window& window = project.GetWindow();
	GLFWwindow* glfw_window = window.GetNativeWindow();
	
	if (!player || !player->HasComponent<CameraComponent>())
		return;

	if (input.GetInputType() == InputType::MouseButtonPressed)
	{
		const auto mouse_button_input = dynamic_cast<MouseButtonPressedInput&>(input);
		if (mouse_button_input.GetMouseButton() == InputCode::ButtonLeft && !is_attacking)
		{
			is_attacking = true;
			attack_timer = 0.0f;
		}
	}

	if (input.GetInputType() == InputType::KeyPressed)
	{
		const auto key_input = dynamic_cast<KeyPressedInput&>(input);
		if (key_input.GetKeyCode() == InputCode::Escape)
		{
			int current_mode = glfwGetInputMode(glfw_window, GLFW_CURSOR);
			if (current_mode == GLFW_CURSOR_DISABLED)
				glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else
				glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	
	if (input.GetInputType() == InputType::MouseMoved)
	{
		auto& mouse_input = dynamic_cast<MouseMovedInput&>(input);
		auto& camera_comp = player->GetComponent<CameraComponent>();
		Camera* camera = camera_comp.camera.get();

		float x = mouse_input.GetX();
		float y = mouse_input.GetY();

		if (camera->IsFirstMouse())
		{
			camera->GetLastX() = x;
			camera->GetLastY() = y;
			camera->IsFirstMouse() = false;
		}

		float x_offset = x - camera->GetLastX();
		float y_offset = y - camera->GetLastY();

		camera->GetLastX() = x;
		camera->GetLastY() = y;

		const float sensitivity = 0.1f;
		x_offset *= sensitivity;
		y_offset *= sensitivity;

		camera->yaw += x_offset;
		camera->pitch -= y_offset;

		camera->pitch = (std::max)(-89.0f, (std::min)(89.0f, camera->pitch));

		camera->UpdateCameraVectors();
	}
}