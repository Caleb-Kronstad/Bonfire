#include "MainLayer.hpp"

MainLayer::MainLayer() : Layer("New Layer")
{

}
MainLayer::~MainLayer()
{

}

void MainLayer::OnAttach()
{
	Project& project = Project::GetInstance();
	Renderer& renderer = Project::GetRenderer();
	PhysicsSystem& physics_system = Project::GetPhysicsSystem();
	AudioSystem& audio_system = Project::GetAudioSystem();
	ScriptSystem& script_system = Project::GetScriptSystem();
	Scene& scene = renderer.GetScene();

	player = scene.GetEntityByName("Player");
	if (!player)
		Log::Error("Failed to find player entity");
}

void MainLayer::OnDetach()
{

}

void MainLayer::OnUpdate(const float& delta_time)
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

	glm::vec3 forward = camera->GetFrontVector();
	forward.y = 0.0f;
	if (glm::length(forward) > 0.0f)
		forward = glm::normalize(forward);

	glm::vec3 right = camera->GetRightVector();
	right = glm::normalize(right);

	glm::vec3 move_dir(0.0f);

	if (glfwGetKey(glfw_window, InputCode::W) == GLFW_PRESS)
		move_dir += forward;
	if (glfwGetKey(glfw_window, InputCode::S) == GLFW_PRESS)
		move_dir -= forward;
	if (glfwGetKey(glfw_window, InputCode::A) == GLFW_PRESS)
		move_dir -= right;
	if (glfwGetKey(glfw_window, InputCode::D) == GLFW_PRESS)
		move_dir += right;

	glm::vec3 current_velocity = body->GetLinearVelocity();
	if (glm::length(move_dir) > 0.0f)
	{
		move_dir = glm::normalize(move_dir);
		float movement_speed = 5.0f;
		glm::vec3 velocity = move_dir * movement_speed;
		velocity.y = current_velocity.y;
		body->SetLinearVelocity(velocity);
	}
	else
	{
		body->SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
	}

	camera->position = body->GetPosition();
}
void MainLayer::OnInterfaceUpdate()
{
	
}
void MainLayer::OnInput(Input& input)
{
	Project& project = Project::GetInstance();
	Window& window = project.GetWindow();
	GLFWwindow* glfw_window = window.GetNativeWindow();
	
	if (!player || !player->HasComponent<CameraComponent>())
		return;

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