#pragma once

#include "Core/Layer.hpp"

#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Entity.hpp"

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
		void OnInterfaceUpdate() override;
		void OnInput(Input& input) override;

	private:
		std::string project_path;
		glm::mat4 manipulation_matrix;

		// engine camera
		std::unique_ptr<Camera> engine_camera;
		bool engine_camera_can_rotate;

		std::vector<std::shared_ptr<Entity>> entities;

		// -- TEST MEMBERS --
		std::unique_ptr<Shader> default_shader;
		std::shared_ptr<Texture> wood_floor_texture;
		std::shared_ptr<Texture> checkered_texture;
		std::shared_ptr<Model> cube_model;
		std::shared_ptr<Model> sphere_model;
		// --
	};
}