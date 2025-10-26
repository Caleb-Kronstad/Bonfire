#include "TestLayer.hpp"

TestLayer::TestLayer() : Layer("New Layer")
{

}
TestLayer::~TestLayer()
{

}

void TestLayer::OnAttach()
{
	Project& project = Project::GetInstance();
	
	player = Project::GetRenderer().GetScene().GetEntities().at(1000005);
	PhysicsComponent& physics_component = player->GetComponent<PhysicsComponent>();
	physics_component.physics_body->SetAllowedDOFS(true, true, true, false, false, false);
}

void TestLayer::OnDetach()
{

}

void TestLayer::OnUpdate(const float& delta_time)
{
	Project& project = Project::GetInstance();
	Renderer& renderer = Project::GetRenderer();
	Camera& camera = *renderer.GetScene().GetCurrentCamera();
	
	if (!project.GetProjectRunState()) return;

	PhysicsComponent& physics_component = player->GetComponent<PhysicsComponent>();

	glm::vec3 move_to = glm::vec3(0.0f);
	if (w_down)
		move_to.x += 10.0f;
	if (s_down)
		move_to.x -= 10.0f;
	if (a_down)
		move_to.z -= 10.0f;
	if (d_down)
		move_to.z += 10.0f;
	if (space_down)
		move_to.y += 1.0f;
	else
		move_to.y -= 1.0f;

	move_to *= (player->GetForwardVector() + player->GetRightVector() + player->GetUpVector());

	glm::vec3 linear_velocity = physics_component.physics_body->GetLinearVelocity();
	move_to.y += linear_velocity.y;
	physics_component.physics_body->SetLinearVelocity(move_to);
	if (max_jump_height < linear_velocity.y)
		space_down = false;

	glm::vec3 camera_offset = glm::vec3(-15.0f, 10.0f, 0.0f);
	glm::vec3 player_no_y = glm::vec3(player->position.x, 0.0f, player->position.z);
	camera.position = camera_offset + player_no_y;
	camera.LookAt(player_no_y);
}
void TestLayer::OnInterfaceUpdate()
{
	
}
void TestLayer::OnInput(Input& input)
{
	switch (input.GetInputType())
	{
	case InputType::KeyPressed:
		{
			const auto key_input = dynamic_cast<KeyPressedInput&>(input);

			// -- actions here --
			if (key_input.GetKeyCode() == InputCode::Space)
			{
				space_down = true;
			}
			if (key_input.GetKeyCode() == InputCode::W)
			{
				w_down = true;
			}
			if (key_input.GetKeyCode() == InputCode::S)
			{
				s_down = true;
			}
			if (key_input.GetKeyCode() == InputCode::A)
			{
				a_down = true;
			}
			if (key_input.GetKeyCode() == InputCode::D)
			{
				d_down = true;
			}
			
			break;
		}
	case InputType::KeyReleased:
		{
			const auto key_input = dynamic_cast<KeyReleasedInput&>(input);

			// -- actions here --
			if (key_input.GetKeyCode() == InputCode::Space)
			{
				space_down = false;
			}
			if (key_input.GetKeyCode() == InputCode::W)
			{
				w_down = false;
			}
			if (key_input.GetKeyCode() == InputCode::S)
			{
				s_down = false;
			}
			if (key_input.GetKeyCode() == InputCode::A)
			{
				a_down = false;
			}
			if (key_input.GetKeyCode() == InputCode::D)
			{
				d_down = false;
			}
			
			break;
		}
	case InputType::KeyTyped:
		{
			const auto key_input = dynamic_cast<KeyTypedInput&>(input);

			// -- actions here --

			break;
		}
	case InputType::MouseButtonPressed:
		{
			const auto mouse_input = dynamic_cast<MouseButtonPressedInput&>(input);
			
			// -- actions here --
			
			break;
		}
	case InputType::MouseButtonReleased:
		{
			const auto mouse_input = dynamic_cast<MouseButtonReleasedInput&>(input);
			
			// -- actions here --
			
			break;
		}
	case InputType::MouseMoved:
		{
			const auto mouse_input = dynamic_cast<MouseMovedInput&>(input);

			// -- actions here --
			
			break;
		}
	case InputType::MouseScrolled:
		{
			const auto mouse_input = dynamic_cast<MouseScrolledInput&>(input);

			// -- actions here --
			
			break;
		}
	case InputType::None:
			break;
	}
}