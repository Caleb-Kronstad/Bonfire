#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
	Renderer::Renderer()
	{
		m_ProjectPath = std::filesystem::current_path().generic_string();
		m_ManipulationMatrix = new glm::mat4(1.0f);
		m_EngineCamera = new Camera(glm::vec3(0.0f, 3.0f, 0.0f));
		m_TestShader = new Shader("Default", "Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
	}
	Renderer::~Renderer()
	{

	}

	void Renderer::OnAttach()
	{
	}
	void Renderer::OnDetach()
	{
	}

	void Renderer::OnUpdate()
	{
		Project& project = Project::GetInstance();
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = m_EngineCamera->GetProjectionMatrix()
	}

	void Renderer::OnInput(Input& input)
	{
	}
}
