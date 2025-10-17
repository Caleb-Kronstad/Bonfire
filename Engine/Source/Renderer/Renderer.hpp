#pragma once

#include "Core/Layer.hpp"

#include "Interface/ConsoleCapture.hpp"
#include "Framebuffer.hpp"

#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Entity.hpp"
#include "Renderer/Scene.hpp"

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

		bool Load();
		bool Save();

		void DrawActiveTitleLine(const ImVec4& active_color, const ImVec4& inactive_color, float thickness = 3.0f);
		void RenderEntityTree(std::shared_ptr<Entity> entity);
		void DuplicateEntity(std::shared_ptr<Entity> entity);
		void DeleteEntity(std::shared_ptr<Entity> entity);

	private:
		std::string project_path;
		glm::mat4 manipulation_matrix;

		// camera
		bool engine_camera_can_rotate;

		// scene
		std::unique_ptr<Scene> scene;
		std::shared_ptr<Entity> selected_entity;
		std::shared_ptr<Entity> entity_to_delete;
		std::unique_ptr<ParamDatabase> param_database;
		std::string new_model_path;
		std::string new_texture_path;
		std::string new_shader_vert_path;
		std::string new_shader_frag_path;
		std::string new_shader_geom_path;

		// interface
		glm::vec4 background_color; // add customization later
		std::unique_ptr<ConsoleCapture> console_capture;
		std::unique_ptr<Framebuffer> viewport_framebuffer;
		glm::vec2 viewport_size = { 1280, 720 };
		float drag_step = 1.0f;
		bool viewport_focused = false;

		// fonts
		ImFont* font_title;
		ImFont* font_body;
	};
}
