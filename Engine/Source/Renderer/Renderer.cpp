#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
	
	Renderer::Renderer()
	{
	}
	Renderer::~Renderer()
	{
		
	}

	void Renderer::OnAttach()
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Editor& project_editor = Project::GetEditor();
		
		background_color = project_editor.GetBackgroundColor();
		manipulation_matrix = glm::mat4(1.0f);

		param_database = std::make_unique<ParamDatabase>("Data/Params/models.params", "Data/Params/textures.params", "Data/Params/shaders.params", "Data/Params/materials.params");
		
		std::stringstream path_stream;
		path_stream << "Project Path: " << std::filesystem::current_path();
		Log::Info(path_stream.str());

		editor_viewport_framebuffer = std::make_unique<Framebuffer>(editor_viewport_size.x, editor_viewport_size.y);
		project_viewport_framebuffer = std::make_unique<Framebuffer>(project_viewport_size.x, project_viewport_size.y);
		
		// SCENE AND EDITOR LOADING
		scene = std::make_unique<Scene>("Data/Scenes/testscene.bonfirescene");
		Load();

		// what is this even for?
		for (auto& [shader_id, shader] : scene->GetShaders())
		{
			shader->Use();
			shader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));
		}
	}
	void Renderer::OnDetach()
	{
	}

	void Renderer::OnUpdate(const float& delta_time)
	{
		Project& project = Project::GetInstance();
		Editor& editor = Project::GetEditor();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Window& project_window = project.GetWindow();
		GLFWwindow* glfw_window = project_window.GetNativeWindow();

		if (editor.EditorViewportVisible())
			RenderEditorViewport();
		if (editor.ProjectViewportVisible())
			RenderProjectViewport();
	}

	void Renderer::RenderEditorViewport()
	{
		Project& project = Project::GetInstance();
		Editor& editor = Project::GetEditor();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Window& project_window = project.GetWindow();
		GLFWwindow* glfw_window = project_window.GetNativeWindow();

		switch (debug_type)
		{
			case DebugType::DEFAULT:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			}
			case DebugType::WIREFRAME:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			}
			case DebugType::POINT:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
			}
		}

		// --- TESTING - IMPROVE IMPLEMENTATION AT LATER TIME ---
		if (project_window.GetWidth() <= 0 || project_window.GetHeight() <= 0)
			return;
		// ---

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = editor.GetEngineCamera().GetProjectionMatrix(editor_viewport_size.x, editor_viewport_size.y);
		glm::mat4 view = editor.GetEngineCamera().GetViewMatrix();
		
		if (scene->GetDirectionalLight() != nullptr && scene->GetDirectionalLight()->enabled)
		{
			scene->GetShadowMap()->LoadDirectional(scene->GetDirectionalLight()->direction);
			scene->GetShadowMap()->SetDirectional();

			for (auto& [shadow_entity_id, shadow_entity] : scene->GetEntities())
				shadow_entity->Draw(editor.GetEngineCamera(), scene->GetShadowMap()->shadow_map_shader, *scene, manipulation_matrix, view, projection);

			scene->GetShadowMap()->Reset(false);
			glViewport(0, 0, editor_viewport_size.x, editor_viewport_size.y);
		}
		
		bool shadow_rendered = false;
		for (auto& [entity_id, entity] : scene->GetEntities())
		{
			if (!shadow_rendered && entity->HasComponent<LightSourceComponent>())
			{
				LightSourceComponent& light_source_component = entity->GetComponent<LightSourceComponent>();
				if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
				{
					scene->GetShadowMap()->Load(point_light->position);
					scene->GetShadowMap()->Set(point_light->position);

					for (auto& [shadow_entity_id, shadow_entity] : scene->GetEntities())
						shadow_entity->Draw(editor.GetEngineCamera(), scene->GetShadowMap()->point_shadow_map_shader, *scene, manipulation_matrix, view, projection);

					scene->GetShadowMap()->Reset(true);
					shadow_rendered = true; 
					break;
				}
			}
		}
		
		editor_viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = editor.GetEngineCamera().GetProjectionMatrix(editor_viewport_size.x, editor_viewport_size.y);
		view = editor.GetEngineCamera().GetViewMatrix();

		scene->GetShadowMap()->updated_this_frame = false;
		for (auto& [shader_id, shader] : scene->GetShaders())
			shader->updated_this_frame = false;
		
		for (auto& [entity_id, entity] : scene->GetEntities())
		{
			if (entity->HasComponent<ModelComponent>())
			{
				ModelComponent& model_component = entity->GetComponent<ModelComponent>();
				entity->Draw(editor.GetEngineCamera(), model_component.shader, *scene, manipulation_matrix, view, projection);
			}
			if (entity->HasComponent<PhysicsComponent>() && project.GetProjectRunState())
			{
				PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
				std::shared_ptr<PhysicsBody> physics_body = physics_component.physics_body;
				entity->position = physics_body->GetPosition();
				entity->rotation = glm::degrees(glm::eulerAngles(physics_body->GetRotation()));
			}
		}

		projection = editor.GetEngineCamera().GetProjectionMatrix(editor_viewport_size.x, editor_viewport_size.y);
		view = editor.GetEngineCamera().GetViewMatrix();
		scene->GetSkybox()->Draw(view, projection);

		editor_viewport_framebuffer->Unbind();
		glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());
	}

	void Renderer::RenderProjectViewport()
	{
		Project& project = Project::GetInstance();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Window& project_window = project.GetWindow();
		GLFWwindow* glfw_window = project_window.GetNativeWindow();

		// --- TESTING - IMPROVE IMPLEMENTATION AT LATER TIME ---
		if (project_window.GetWidth() <= 0 || project_window.GetHeight() <= 0)
			return;
		// ---

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = scene->GetCurrentCamera()->GetProjectionMatrix(project_viewport_size.x, project_viewport_size.y);
		glm::mat4 view = scene->GetCurrentCamera()->GetViewMatrix();
		
		if (scene->GetDirectionalLight() != nullptr && scene->GetDirectionalLight()->enabled)
		{
			scene->GetShadowMap()->LoadDirectional(scene->GetDirectionalLight()->direction);
			scene->GetShadowMap()->SetDirectional();

			for (auto& [shadow_entity_id, shadow_entity] : scene->GetEntities())
				shadow_entity->Draw(*scene->GetCurrentCamera(), scene->GetShadowMap()->shadow_map_shader, *scene, manipulation_matrix, view, projection);

			scene->GetShadowMap()->Reset(false);
			glViewport(0, 0, project_viewport_size.x, project_viewport_size.y);
		}
		
		bool shadow_rendered = false;
		for (auto& [entity_id, entity] : scene->GetEntities())
		{
			if (!shadow_rendered && entity->HasComponent<LightSourceComponent>())
			{
				LightSourceComponent& light_source_component = entity->GetComponent<LightSourceComponent>();
				if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
				{
					scene->GetShadowMap()->Load(point_light->position);
					scene->GetShadowMap()->Set(point_light->position);

					for (auto& [shadow_entity_id, shadow_entity] : scene->GetEntities())
						shadow_entity->Draw(*scene->GetCurrentCamera(), scene->GetShadowMap()->point_shadow_map_shader, *scene, manipulation_matrix, view, projection);

					scene->GetShadowMap()->Reset(true);
					shadow_rendered = true; 
					break;
				}
			}
		}
		
		project_viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = scene->GetCurrentCamera()->GetProjectionMatrix(project_viewport_size.x, project_viewport_size.y);
		view = scene->GetCurrentCamera()->GetViewMatrix();

		scene->GetShadowMap()->updated_this_frame = false;
		for (auto& [shader_id, shader] : scene->GetShaders())
			shader->updated_this_frame = false;
		
		for (auto& [entity_id, entity] : scene->GetEntities())
		{
			if (entity->HasComponent<ModelComponent>())
			{
				ModelComponent& model_component = entity->GetComponent<ModelComponent>();
				entity->Draw(*scene->GetCurrentCamera(), model_component.shader, *scene, manipulation_matrix, view, projection);
			}
			if (entity->HasComponent<PhysicsComponent>() && project.GetProjectRunState())
			{
				PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
				std::shared_ptr<PhysicsBody> physics_body = physics_component.physics_body;
				entity->position = physics_body->GetPosition();
				entity->rotation = glm::degrees(glm::eulerAngles(physics_body->GetRotation()));
			}
		}

		projection = scene->GetCurrentCamera()->GetProjectionMatrix(project_viewport_size.x, project_viewport_size.y);
		view = scene->GetCurrentCamera()->GetViewMatrix();
		scene->GetSkybox()->Draw(view, projection);

		project_viewport_framebuffer->Unbind();
		glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());
	}


	bool Renderer::Load()
	{
		bool params_loaded = param_database->LoadParams();
		bool scene_loaded = scene->LoadScene(*param_database);
		return scene_loaded || params_loaded;
	}
	bool Renderer::Save()
	{
		bool scene_saved = scene->SaveScene(*param_database);
		bool params_saved = param_database->SaveParams(scene->GetMaterials());
		return scene_saved || params_saved;
	}
}
