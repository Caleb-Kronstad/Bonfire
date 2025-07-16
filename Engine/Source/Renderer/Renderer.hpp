#pragma once

#include "Core/Layer.hpp"

#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Model.hpp"

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
		std::string project_path;
		glm::mat4 manipulation_matrix;
		
		std::unique_ptr<Camera> engine_camera;

		// -- TEST MEMBERS --
		std::unique_ptr<Shader> test_shader;
		std::shared_ptr<Texture> test_texture;
		std::unique_ptr<Model> test_model;
		// --
	};
}