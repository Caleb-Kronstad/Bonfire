#include "Player.hpp"

Player::Player() : Layer("New Layer")
{
	player_stats = PlayerStats();
	current_health = player_stats.max_health;
	weapon_stats = WeaponStats("Greatsword", glm::vec3(-1.0f, 0.1f, 0.0f), 5, 1, 10);
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

	player = scene.GetEntityOfName("Player");
	arm = scene.GetEntityOfName("PlayerArm");
	weapon = scene.GetEntityOfName(weapon_stats.name);
	
	music_audio = scene.GetEntityOfName("GlobalMusic")->GetComponent<AudioComponent>().audio;
	music_audio->SetSpatialization(false);
	music_audio->Play();

	if (player->HasComponent<PhysicsComponent>())
	{
		PhysicsComponent& physics_component = player->GetComponent<PhysicsComponent>();
		physics_system.RegisterBodyEntity(physics_component.physics_body->GetBodyID(), player);
	}

	spawn = player->position;
}

void Player::OnDetach()
{
	music_audio->Stop();
}

void Player::OnUpdate(const float& delta_time)
{
	Project& project = Project::GetInstance();
	Window& window = project.GetWindow();
	GLFWwindow* glfw_window = window.GetNativeWindow();
	
	if (!player || !player->HasComponent<CameraComponent>() || !player->HasComponent<PhysicsComponent>())
		return;

	CameraComponent& camera_comp = player->GetComponent<CameraComponent>();
	PhysicsComponent& physics_comp = player->GetComponent<PhysicsComponent>();
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

	// update arm and weapon rotation
    glm::quat camera_rotation = glm::quat(glm::vec3(0.0f, glm::radians(-camera->yaw + 180.0f), 0.0f));
    glm::quat attack_rotation = glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(attack_angle)));
    glm::quat final_arm_rotation = camera_rotation * attack_rotation;
    arm->rotation = glm::degrees(glm::eulerAngles(final_arm_rotation));
    glm::vec3 rotated_weapon_offset = final_arm_rotation * weapon_stats.offset;
    weapon->position = arm->position + rotated_weapon_offset;
	glm::quat weapon_local_rotation = glm::quat(glm::radians(glm::vec3(270.0f, 270.0f, 0.0f)));
	glm::quat final_weapon_rotation = final_arm_rotation * weapon_local_rotation;
    weapon->rotation = glm::degrees(glm::eulerAngles(final_weapon_rotation));

	// update player entity rotation
    player->rotation = glm::vec3(0.0f, glm::radians(camera->yaw), 0.0f);

	// update camera position
	bool is_moving = glm::length(move_direction) > 0.0f;
	glm::vec3 bob_offset = CalculateCameraBob(delta_time, is_moving);
	camera->position = body->GetPosition() + camera_offset + bob_offset;

	for (int i = recent_hitbox_cooldowns.size() - 1; i >= 0; i--)
	{
		recent_hitbox_cooldowns[i].timer -= delta_time;
		if (recent_hitbox_cooldowns[i].timer <= 0.0f)
		{
			recent_hitbox_cooldowns.erase(recent_hitbox_cooldowns.begin() + i);
		}
	}
}

void Player::OnInterfaceUpdate()
{
	Renderer& renderer = Project::GetRenderer();
	Scene& scene = renderer.GetScene();

	DrawHealthBar();
	
	// -- DEBUG --
	int current_scene_index = -1;
	if (scene.path.find("calcifiedvillage") != std::string::npos)
		current_scene_index = 0;
	else if (scene.path.find("thehollow") != std::string::npos)
		current_scene_index = 1;

	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (current_scene_index == 0)
		ImGui::Text("Current Scene: Calcified Village");
	else if (current_scene_index == 1)
		ImGui::Text("Current Scene: The Hollow");
	else if (current_scene_index == 2)
		ImGui::Text("Current Scene: Unknown");

	ImGui::Separator();

	if (current_scene_index == 0)
	{
		if (ImGui::Button("Go to The Hollow", ImVec2(200, 22)))
			renderer.NextScene(1);
	}
	else if (current_scene_index == 1)
	{
		if (ImGui::Button("Go to Calcified Village", ImVec2(200, 22)))
			renderer.NextScene(0);
	}

	ImGui::End();
	// --
}

void Player::DrawHealthBar()
{
	Window& window = Project::GetInstance().GetWindow();

	float screen_width = static_cast<float>(window.GetWidth());
	float screen_height = static_cast<float>(window.GetHeight());
	float bar_width = screen_width * 0.15f;
	float bar_height = screen_height * 0.03f;
	float offset_x = screen_width * 0.1f;
	float offset_y = screen_height * 0.1f;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
									 ImGuiWindowFlags_NoBackground |
									 ImGuiWindowFlags_NoMove |
									 ImGuiWindowFlags_NoInputs |
									 ImGuiWindowFlags_NoSavedSettings |
									 ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::SetNextWindowPos(ImVec2(offset_x, offset_y), ImGuiCond_Always);
	ImGui::Begin("##HealthBar", nullptr, window_flags);
	float health_percentage = current_health / player_stats.max_health;
	health_percentage = glm::clamp(health_percentage, 0.0f, 1.0f);

	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImGui::Dummy(ImVec2(bar_width, bar_height));
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImU32 bg_color = IM_COL32(80, 80, 80, 255);
	ImU32 health_color = IM_COL32(220, 20, 20, 255);
	ImU32 border_color = IM_COL32(0, 0, 0, 255);

	draw_list->AddRectFilled(cursor_pos, ImVec2(cursor_pos.x + bar_width, cursor_pos.y + bar_height), bg_color, 0.0f);
	float filled_width = bar_width * health_percentage;
	if (filled_width > 0.0f)
		draw_list->AddRectFilled(cursor_pos, ImVec2(cursor_pos.x + filled_width, cursor_pos.y + bar_height), health_color, 0.0f);

	//draw_list->AddRect(cursor_pos, ImVec2(cursor_pos.x + bar_width, cursor_pos.y + bar_height), border_color, 0.0f);

	ImGui::End();
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

void Player::TakeDamage(float damage, std::shared_ptr<Entity> hitbox)
{
	if (!hitbox) return;

	for (const auto& cooldown : recent_hitbox_cooldowns)
	{
		if (cooldown.hitbox == hitbox)
			return;
	}

	current_health -= damage;
	Log::Info("Player took " + std::to_string(damage) + " damage");

	recent_hitbox_cooldowns.push_back(HitboxCooldown(hitbox, DAMAGE_COOLDOWN_TIME));

	if (current_health <= 0.0f)
	{
		current_health = 0.0f;
		Log::Info("Player has no health remaining");
	}
}


glm::vec3 Player::CalculateCameraBob(const float& delta_time, bool is_moving)
{
	if (has_smooth_transition)
	{
		float target_intensity = is_moving ? 1.0f : 0.0f;
		current_bob_intensity = glm::mix(current_bob_intensity, target_intensity, bob_transition_speed * delta_time);
	}
	else
	{
		current_bob_intensity = is_moving ? 1.0f : 0.0f;
	}

	if (current_bob_intensity > 0.01f)
	{
		bob_timer += delta_time * bob_frequency;
	}

	float vertical_bob = glm::sin(bob_timer) * bob_vertical_amplitude;
	float horizontal_bob = glm::cos(bob_timer * 0.5f) * bob_horizontal_amplitude;

	return glm::vec3(horizontal_bob, vertical_bob, 0.0f) * current_bob_intensity;
}
