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
		void ReloadParams(ParamReference ref, PARAM_TYPE type);

		void DrawActiveTitleLine(const ImVec4& color, float thickness = 3.0f);
		
		void DrawModel(ParamReference model_ref, ParamReference texture_ref);
		void DrawEntity(EntityID id);
		glm::quat GetTransformOrientation(EntityID id);
		glm::mat4 GetTransformMatrix(EntityID id);

	private:
		std::string project_path;
		glm::mat4 manipulation_matrix;

		// camera
		std::unique_ptr<Camera> engine_camera;
		bool engine_camera_can_rotate;

		// scenes
		std::unique_ptr<Scene> scene;

		// entities
		std::unique_ptr<ParamDatabase> param_database;
		std::vector<EntityID> entities;
		std::unordered_map<EntityID, EntityData> entities_data;

		// param maps
		std::unordered_map<ParamReference, Model> models;
		std::unordered_map<ParamReference, Texture> textures;

		// interface
		std::unique_ptr<ConsoleCapture> console_capture;
		std::unique_ptr<Framebuffer> viewport_framebuffer;
		glm::vec2 viewport_size = { 1280, 720 };
		float drag_step = 1.0f;
		bool viewport_focused = false;
		EntityID current_entity_id = EntityID(0);

		// fonts
		ImFont* font_title;
		ImFont* font_body;

		// -- TEST MEMBERS --
		std::unique_ptr<Shader> default_shader;
		glm::vec4 background_color;
		// --
	};
}
