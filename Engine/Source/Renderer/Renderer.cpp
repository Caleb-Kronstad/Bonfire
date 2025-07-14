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
		m_TestShader = new Shader("Resources/Shaders/default.vert", "Resources/Shaders/default.frag", "None");
	}
	Renderer::~Renderer()
	{

	}

	void Renderer::OnAttach()
	{
		m_TestShader->Use();
		m_TestShader->SetVec4("color", glm::vec4(0.0f, 1.0f, 0.9f, 1.0f));
	}
	void Renderer::OnDetach()
	{
	}

	void Renderer::OnUpdate()
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = m_EngineCamera->GetProjectionMatrix(window.GetWidth(), window.GetHeight());
		glm::mat4 view = m_EngineCamera->GetViewMatrix();

		m_TestShader->Use();
		m_TestShader->SetMat4("projection", projection);
		m_TestShader->SetMat4("view", view);
		m_TestShader->SetMat4("model", glm::mat4(1.0f));
	}

	void Renderer::OnInput(Input& input)
	{
	}
}
