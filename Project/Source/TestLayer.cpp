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
	
	player = Project::GetRenderer().scene->GetEntities().at(1000005);
	PhysicsComponent& physics_component = player->GetComponent<PhysicsComponent>();
	physics_component.physics_body->SetAllowedDOFS(true, true, true, false, false, false);
}

void TestLayer::OnDetach()
{

}

void TestLayer::OnUpdate()
{
	Project& project = Project::GetInstance();
	Renderer& renderer = Project::GetRenderer();
	Camera& camera = *renderer.scene->GetEngineCamera();
	
	if (!project.GetProjectRunState()) return;

	PhysicsComponent& physics_component = player->GetComponent<PhysicsComponent>();

	float move_x = 0.0f;
	float move_z = 0.0f;
	float move_y = 0.0f;
	if (w_down)
		move_x += 10.0f;
	if (s_down)
		move_x -= 10.0f;
	if (a_down)
		move_z -= 10.0f;
	if (d_down)
		move_z += 10.0f;
	if (space_down)
		move_y += 1.0f;
	else
		move_y -= 1.0f;

	glm::vec3 linear_velocity = physics_component.physics_body->GetLinearVelocity();
	physics_component.physics_body->SetLinearVelocity(glm::vec3(move_x, linear_velocity.y + move_y, move_z));
	if (max_jump_height < linear_velocity.y)
		space_down = false;

	glm::vec3 camera_offset = glm::vec3(-15.0f, 10.0f, 0.0f);
	glm::vec3 player_no_y = glm::vec3(player->position.x, 0.0f, player->position.z);
	camera.position = camera_offset + player_no_y;
	camera.LookAt(player_no_y);
}
void TestLayer::OnInterfaceUpdate()
{
	Project& project = Project::GetInstance();
	if (!project.GetProjectRunState()) return;
	
}
void TestLayer::OnInput(Input& input)
{
	Project& project = Project::GetInstance();
	if (!project.GetProjectRunState()) return;

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