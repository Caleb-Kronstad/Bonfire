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

		bool Load();
		bool Save();

		bool AddScene(std::unique_ptr<Scene> scene);
		bool RemoveScene(std::unique_ptr<Scene> scene);
		void NextScene(int scene_index);
		const std::vector<std::unique_ptr<Scene>>& GetScenes() { return scenes; }

		void RenderEditorViewport(const float& delta_time);
		void RenderProjectViewport(const float& delta_time);
		void DrawColliders(const glm::mat4& projection, const glm::mat4& view);
		void DrawEntity(std::shared_ptr<Entity> entity, Camera& camera);

		Scene& GetScene() const { return *scenes.at(current_scene_index); }
		ParamDatabase& GetParamDatabase() const { return *param_database; }
		glm::vec2& GetEditorViewportSize() { return editor_viewport_size; }
		glm::vec2& GetProjectViewportSize() { return project_viewport_size; }
		Framebuffer& GetEditorViewportFramebuffer() const { return *editor_viewport_framebuffer; }
		Framebuffer& GetProjectViewportFramebuffer() const { return *project_viewport_framebuffer; }
		DebugType& GetDebugType() { return debug_type; }
		bool& GetDrawColliders() { return draw_colliders; }
		bool& GetDrawMeshColliders() { return draw_mesh_colliders; }
		float& GetDrawCollidersLineWidth() { return draw_colliders_line_width; }
		std::shared_ptr<Shader>& GetInstancedShader() { return instanced_shader; }
		const int& GetCurrentSceneIndex() { return current_scene_index; }

	private:
		glm::mat4 projection = glm::mat4(0.0f);
		glm::mat4 view = glm::mat4(0.0f);
        glm::vec4 background_color;
        DebugType debug_type = DebugType::DEFAULT;
		bool draw_colliders = false;
		bool draw_mesh_colliders = false;
		float draw_colliders_line_width = 1.0f;
		
		// scene
		std::vector<std::unique_ptr<Scene>> scenes;
		int current_scene_index = 0;
		std::unique_ptr<ParamDatabase> param_database;
        std::unique_ptr<Framebuffer> editor_viewport_framebuffer;
		std::unique_ptr<Framebuffer> project_viewport_framebuffer;
        glm::vec2 editor_viewport_size = { 1280, 720 };
		glm::vec2 project_viewport_size = { 1280, 720 };

		std::shared_ptr<Shader> instanced_shader;
		std::map<std::shared_ptr<Model>, std::vector<glm::mat4>> batches;
		std::map<std::shared_ptr<Model>, GLuint> instance_buffers;
		std::map<std::shared_ptr<Model>, std::shared_ptr<Material>> batch_materials;
	};
}
