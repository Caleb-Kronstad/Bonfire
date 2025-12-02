#pragma once

#include "Core/Layer.hpp"

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Ray.hpp"
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
		void OnUpdate(const float& delta_time) override;

		bool AddScene(std::unique_ptr<Scene> scene);
		bool RemoveScene(std::unique_ptr<Scene> scene);
		void LoadScene(int scene_index);
		Scene& GetScene() const { return *scenes.at(current_scene_index); }
		const std::vector<std::unique_ptr<Scene>>& GetScenes() { return scenes; }

		void RenderViewport(const float& delta_time, Camera& camera, Framebuffer& framebuffer, glm::vec2 viewport_size);
		void RenderModelPreview(std::shared_ptr<Model> model, std::shared_ptr<Material> material, std::shared_ptr<Shader> shader, Framebuffer& framebuffer, glm::vec3 rotation_angle);
		void DrawColliders(const glm::mat4& projection, const glm::mat4& view);
		void DrawEntity(std::shared_ptr<Entity> entity, Camera& camera);

		ParamDatabase& GetParamDatabase() const { return *param_database; }
		glm::vec2& GetProjectViewportSize() { return project_viewport_size; }
		Framebuffer& GetProjectViewportFramebuffer() const { return *project_viewport_framebuffer; }
		DebugType& GetDebugType() { return debug_type; }
		bool& GetDrawColliders() { return draw_colliders; }
		bool& GetDrawMeshColliders() { return draw_mesh_colliders; }
		float& GetDrawCollidersLineWidth() { return draw_colliders_line_width; }
		const int& GetCurrentSceneIndex() { return current_scene_index; }

		bool GetProjectViewportFocused() const { return project_viewport_focused; }
		bool GetProjectViewportHovered() const { return project_viewport_hovered; }
		bool GetProjectViewportVisible() const { return project_viewport_visible; }
		void SetProjectViewportFocused(bool state) { project_viewport_focused = state; }
		void SetProjectViewportHovered(bool state) { project_viewport_hovered = state; }
		void SetProjectViewportVisible(bool state) { project_viewport_visible = state; }

	private:
		glm::mat4 projection = glm::mat4(0.0f);
		glm::mat4 view = glm::mat4(0.0f);
        glm::vec4 background_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        DebugType debug_type = DebugType::DEFAULT;
		bool draw_colliders = false;
		bool draw_mesh_colliders = false;
		float draw_colliders_line_width = 1.0f;
		
		std::unique_ptr<Framebuffer> project_viewport_framebuffer;
		glm::vec2 project_viewport_size = { 1280, 720 };
		bool project_viewport_focused = false;
		bool project_viewport_hovered = false;
		bool project_viewport_visible = false;
		
		// scene
		std::vector<std::unique_ptr<Scene>> scenes;
		int current_scene_index = 0;
		bool scene_transition_in_progress = false;
		std::unique_ptr<ParamDatabase> param_database;
	};
}
