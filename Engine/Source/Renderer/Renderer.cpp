#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Utility.hpp"
#include "Core/Project.hpp"
#include "Editor/Debug.hpp"

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

		param_database = std::make_unique<ParamDatabase>("Data/Params/models.params", "Data/Params/textures.params", "Data/Params/shaders.params", "Data/Params/materials.params", "Data/Params/audios.params");
		
		std::stringstream path_stream;
		path_stream << "Project Path: " << std::filesystem::current_path();
		Log::Info(path_stream.str());

		editor_viewport_framebuffer = std::make_unique<Framebuffer>(editor_viewport_size.x, editor_viewport_size.y);
		project_viewport_framebuffer = std::make_unique<Framebuffer>(project_viewport_size.x, project_viewport_size.y);
		
		// SCENE AND EDITOR LOADING
		scene = std::make_unique<Scene>("Data/Scenes/testscene.bonfire");
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

		if (project.GetProjectRunState() || editor.PreviewAnimations())
		{
			for (auto& [entity_id, entity] : scene->GetEntities())
			{
				if (entity->HasComponent<AnimationComponent>())
				{
					AnimationComponent& animation_component = entity->GetComponent<AnimationComponent>();
					if (animation_component.animator)
						animation_component.animator->Update(delta_time);
				}
				if (entity->HasComponent<AudioComponent>())
				{
					AudioComponent& audio_component = entity->GetComponent<AudioComponent>();
					if (audio_component.audio && audio_component.enabled)
					{
						audio_component.audio->Set3DPosition(entity->position);
						if (audio_component.audio->GetPlayOnAwake() && !audio_component.audio->IsPlaying())
							audio_component.audio->Play();
					}
				}
			}
		}

		Project::GetAudioSystem().UpdateListener(editor.GetEngineCamera().position, editor.GetEngineCamera().GetFrontVector(), editor.GetEngineCamera().GetUpVector());
		//Project::GetAudioSystem().UpdateListener(scene->GetCurrentCamera()->position, scene->GetCurrentCamera()->GetFrontVector(), scene->GetCurrentCamera()->GetUpVector());

		if (editor.EditorViewportVisible())
			RenderEditorViewport(delta_time);
		if (editor.ProjectViewportVisible())
			RenderProjectViewport(delta_time);
	}

	void Renderer::RenderEditorViewport(const float& delta_time)
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
			{
				if (shadow_entity->HasComponent<ModelComponent>())
				{
					ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
					if (model_component.model->casts_shadow)
					{
						shadow_entity->Draw(editor.GetEngineCamera(), scene->GetShadowMap()->shadow_map_shader, *scene, manipulation_matrix, view, projection);
					}
				}
			}

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
					{
						if (shadow_entity->HasComponent<ModelComponent>())
						{
							ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
							if (model_component.model->casts_shadow)
							{
								shadow_entity->Draw(editor.GetEngineCamera(), scene->GetShadowMap()->point_shadow_map_shader, *scene, manipulation_matrix, view, projection);
							}
						}
					}

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
				ModelComponent& model_component = entity->GetComponent<ModelComponent>();;
				entity->Draw(*scene->GetCurrentCamera(), model_component.shader, *scene, manipulation_matrix, view, projection);
			}
		}

		DrawColliders(projection, view);

		projection = editor.GetEngineCamera().GetProjectionMatrix(editor_viewport_size.x, editor_viewport_size.y);
		view = editor.GetEngineCamera().GetViewMatrix();
		scene->GetSkybox()->Draw(view, projection);

		editor_viewport_framebuffer->Unbind();
		glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());
	}

	void Renderer::RenderProjectViewport(const float& delta_time)
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
			{
				if (shadow_entity->HasComponent<ModelComponent>())
				{
					ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
					if (model_component.model->casts_shadow)
					{
						shadow_entity->Draw(*scene->GetCurrentCamera(), scene->GetShadowMap()->shadow_map_shader, *scene, manipulation_matrix, view, projection);
					}
				}
			}

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
					{
						if (shadow_entity->HasComponent<ModelComponent>())
						{
							ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
							if (model_component.model->casts_shadow)
							{
								shadow_entity->Draw(*scene->GetCurrentCamera(), scene->GetShadowMap()->point_shadow_map_shader, *scene, manipulation_matrix, view, projection);
							}
						}
					}

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
		}

		projection = scene->GetCurrentCamera()->GetProjectionMatrix(project_viewport_size.x, project_viewport_size.y);
		view = scene->GetCurrentCamera()->GetViewMatrix();
		scene->GetSkybox()->Draw(view, projection);

		project_viewport_framebuffer->Unbind();
		glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());
	}

	void Renderer::DrawColliders(const glm::mat4& projection, const glm::mat4& view)
	{
		if (!draw_colliders) return;

		std::shared_ptr<Shader> debug_shader = nullptr;
		for (auto& [id, shader] : scene->GetShaders())
		{
			if (shader->name == "Debug")
			{
				debug_shader = shader;
				break;
			}
		}

		if (!debug_shader) return;

		glDisable(GL_DEPTH_TEST);
		glLineWidth(draw_colliders_line_width);

		debug_shader->Use();
		debug_shader->SetMat4("projection", projection);
		debug_shader->SetMat4("view", view);

		unsigned int vertex_array, vertex_buffer;
		glGenVertexArrays(1, &vertex_array);
		glGenBuffers(1, &vertex_buffer);

		glBindVertexArray(vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		for (auto& [id, entity] : scene->GetEntities())
		{
			if (!entity->HasComponent<PhysicsComponent>()) continue;

			PhysicsComponent& physics_comp = entity->GetComponent<PhysicsComponent>();
			PhysicsShapeData shape_data = physics_comp.physics_body->GetShapeData();

			glm::vec3 color;
			switch (physics_comp.physics_body->GetBodyType())
			{
			case PhysicsBodyType::STATIC: color = RgbToGlmVec3(0, 255, 0); break;
			case PhysicsBodyType::DYNAMIC: color = RgbToGlmVec3(0, 255, 0); break;
			case PhysicsBodyType::KINEMATIC: color = RgbToGlmVec3(0, 255, 0); break;
			}
			debug_shader->SetVec4("color", glm::vec4(color, 1.0f));

			glm::vec3 pos = physics_comp.physics_body->GetPosition();
			glm::quat rot = physics_comp.physics_body->GetRotation();
			glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot);
			debug_shader->SetMat4("model", model);
			
			std::vector<glm::vec3> vertices;
			switch (shape_data.type)
			{
			case PhysicsShapeType::BOX:
				vertices = Debug::GetBoxVertices(shape_data.dimensions);
				break;
			case PhysicsShapeType::SPHERE:
				vertices = Debug::GetSphereVertices(shape_data.dimensions.x);
				break;
			case PhysicsShapeType::CAPSULE:
				vertices = Debug::GetCapsuleVertices(shape_data.dimensions.x, shape_data.dimensions.y);
				break;
			}

			if (!vertices.empty())
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
				glDrawArrays(GL_LINES, 0, vertices.size());
			}
		}

		glLineWidth(1.0f);
		glEnable(GL_DEPTH_TEST);
		
		glDeleteVertexArrays(1, &vertex_array);
		glDeleteBuffers(1, &vertex_buffer);
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
