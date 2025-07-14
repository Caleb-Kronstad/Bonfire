#pragma once

#include "Core/Layer.hpp"

#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"

#include "Input/Input.hpp"
#include "Input/InputCodes.hpp"
#include "Input/InputTypes.hpp"

namespace Bonfire
{
	class Renderer : public Layer
	{
	public:
		Renderer();
		~Renderer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnInput(Input& input) override;

	private:
		std::string m_ProjectPath;
		glm::mat4* m_ManipulationMatrix;
		Camera* m_EngineCamera;
		
		Shader* m_TestShader;
	};
}