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
		
		background_color = RgbaToGlmVec4(23, 23, 23);

		param_database = std::make_unique<ParamDatabase>(
			"Data/Params/models.params", "Data/Params/textures.params", "Data/Params/shaders.params",
			"Data/Params/materials.params", "Data/Params/audios.params", "Data/Params/scripts.params");
		
		std::stringstream path_stream;
		path_stream << "Project Path: " << std::filesystem::current_path();
		Log::Info(path_stream.str());

		editor_viewport_framebuffer = std::make_unique<Framebuffer>(editor_viewport_size.x, editor_viewport_size.y);
		project_viewport_framebuffer = std::make_unique<Framebuffer>(project_viewport_size.x, project_viewport_size.y);
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

		if (project.GetProjectRunState())
		{
			for (auto& [entity_id, entity] : scenes.at(current_scene_index)->GetEntities())
			{
				if (entity->HasComponent<AnimationComponent>() && editor.PreviewAnimations())
				{
					AnimationComponent& animation_component = entity->GetComponent<AnimationComponent>();
					if (animation_component.animator)
						animation_component.animator->Update(delta_time);
				}
				if (entity->HasComponent<AudioComponent>() && editor.PreviewAudios())
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

		if (project.GetProjectRunState())
		{
			Project::GetAudioSystem().UpdateListener(scenes.at(current_scene_index)->GetCurrentCamera()->position, scenes.at(current_scene_index)->GetCurrentCamera()->GetFrontVector(), scenes.at(current_scene_index)->GetCurrentCamera()->GetUpVector());
			Project::GetScriptSystem().UpdateScripts(*scenes.at(current_scene_index), delta_time);
		}
		else
		{
			Project::GetAudioSystem().UpdateListener(editor.GetEngineCamera().position, editor.GetEngineCamera().GetFrontVector(), editor.GetEngineCamera().GetUpVector());
		}
		
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
		
		projection = editor.GetEngineCamera().GetProjectionMatrix(editor_viewport_size.x, editor_viewport_size.y);
		view = editor.GetEngineCamera().GetViewMatrix();
		
		if (scenes.at(current_scene_index)->GetDirectionalLight() != nullptr && scenes.at(current_scene_index)->GetDirectionalLight()->enabled)
		{
			scenes.at(current_scene_index)->GetShadowMap()->LoadDirectional(scenes.at(current_scene_index)->GetDirectionalLight()->direction);
			scenes.at(current_scene_index)->GetShadowMap()->SetDirectional();

			for (auto& [shadow_entity_id, shadow_entity] : scenes.at(current_scene_index)->GetEntities())
			{
				if (shadow_entity->HasComponent<ModelComponent>())
				{
					ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
					if (model_component.model->casts_shadow)
					{
						shadow_entity->Draw(scenes.at(current_scene_index)->GetShadowMap()->shadow_map_shader, *scenes.at(current_scene_index));
					}
				}
			}

			scenes.at(current_scene_index)->GetShadowMap()->Reset(false);
			glViewport(0, 0, editor_viewport_size.x, editor_viewport_size.y);
		}
		
		bool shadow_rendered = false;
		for (auto& [entity_id, entity] : scenes.at(current_scene_index)->GetEntities())
		{
			if (!shadow_rendered && entity->HasComponent<LightSourceComponent>())
			{
				LightSourceComponent& light_source_component = entity->GetComponent<LightSourceComponent>();
				if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
				{
					scenes.at(current_scene_index)->GetShadowMap()->Load(point_light->position);
					scenes.at(current_scene_index)->GetShadowMap()->Set(point_light->position);

					for (auto& [shadow_entity_id, shadow_entity] : scenes.at(current_scene_index)->GetEntities())
					{
						if (shadow_entity->HasComponent<ModelComponent>())
						{
							ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
							if (model_component.model->casts_shadow)
							{
								shadow_entity->Draw(scenes.at(current_scene_index)->GetShadowMap()->point_shadow_map_shader, *scenes.at(current_scene_index));
							}
						}
					}

					scenes.at(current_scene_index)->GetShadowMap()->Reset(true);
					break;
				}
			}
		}
		
		editor_viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = editor.GetEngineCamera().GetProjectionMatrix(editor_viewport_size.x, editor_viewport_size.y);
		view = editor.GetEngineCamera().GetViewMatrix();

		// MOVE BELOW ENTITY DRAW LOOP
		scenes.at(current_scene_index)->GetShadowMap()->updated_this_frame = false;
		for (auto& [shader_id, shader] : scenes.at(current_scene_index)->GetShaders())
			shader->updated_this_frame = false;
		
		for (auto& [entity_id, entity] : scenes.at(current_scene_index)->GetEntities())
		{
			entity->UpdateComponents();
			DrawEntity(entity, editor.GetEngineCamera());
		}

		DrawColliders(projection, view);

		projection = editor.GetEngineCamera().GetProjectionMatrix(editor_viewport_size.x, editor_viewport_size.y);
		view = editor.GetEngineCamera().GetViewMatrix();
		scenes.at(current_scene_index)->GetSkybox()->Draw(view, projection);

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
		
		projection = scenes.at(current_scene_index)->GetCurrentCamera()->GetProjectionMatrix(project_viewport_size.x, project_viewport_size.y);
		view = scenes.at(current_scene_index)->GetCurrentCamera()->GetViewMatrix();
		
		if (scenes.at(current_scene_index)->GetDirectionalLight() != nullptr && scenes.at(current_scene_index)->GetDirectionalLight()->enabled)
		{
			scenes.at(current_scene_index)->GetShadowMap()->LoadDirectional(scenes.at(current_scene_index)->GetDirectionalLight()->direction);
			scenes.at(current_scene_index)->GetShadowMap()->SetDirectional();

			for (auto& [shadow_entity_id, shadow_entity] : scenes.at(current_scene_index)->GetEntities())
			{
				if (shadow_entity->HasComponent<ModelComponent>())
				{
					ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
					if (model_component.model->casts_shadow)
					{
						shadow_entity->Draw(scenes.at(current_scene_index)->GetShadowMap()->shadow_map_shader, *scenes.at(current_scene_index));
					}
				}
			}

			scenes.at(current_scene_index)->GetShadowMap()->Reset(false);
			glViewport(0, 0, project_viewport_size.x, project_viewport_size.y);
		}
		
		bool shadow_rendered = false;
		for (auto& [entity_id, entity] : scenes.at(current_scene_index)->GetEntities())
		{
			if (!shadow_rendered && entity->HasComponent<LightSourceComponent>())
			{
				LightSourceComponent& light_source_component = entity->GetComponent<LightSourceComponent>();
				if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
				{
					scenes.at(current_scene_index)->GetShadowMap()->Load(point_light->position);
					scenes.at(current_scene_index)->GetShadowMap()->Set(point_light->position);

					for (auto& [shadow_entity_id, shadow_entity] : scenes.at(current_scene_index)->GetEntities())
					{
						if (shadow_entity->HasComponent<ModelComponent>())
						{
							ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
							if (model_component.model->casts_shadow)
							{
								shadow_entity->Draw(scenes.at(current_scene_index)->GetShadowMap()->point_shadow_map_shader, *scenes.at(current_scene_index));
							}
						}
					}

					scenes.at(current_scene_index)->GetShadowMap()->Reset(true);
					break;
				}
			}
		}
		
		project_viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = scenes.at(current_scene_index)->GetCurrentCamera()->GetProjectionMatrix(project_viewport_size.x, project_viewport_size.y);
		view = scenes.at(current_scene_index)->GetCurrentCamera()->GetViewMatrix();

		scenes.at(current_scene_index)->GetShadowMap()->updated_this_frame = false;
		for (auto& [shader_id, shader] : scenes.at(current_scene_index)->GetShaders())
			shader->updated_this_frame = false;
		
		for (auto& [entity_id, entity] : scenes.at(current_scene_index)->GetEntities())
		{
			entity->UpdateComponents();
			DrawEntity(entity, *scenes.at(current_scene_index)->GetCurrentCamera());
		}

		projection = scenes.at(current_scene_index)->GetCurrentCamera()->GetProjectionMatrix(project_viewport_size.x, project_viewport_size.y);
		view = scenes.at(current_scene_index)->GetCurrentCamera()->GetViewMatrix();
		scenes.at(current_scene_index)->GetSkybox()->Draw(view, projection);

		project_viewport_framebuffer->Unbind();
		glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());
	}

	void Renderer::DrawEntity(std::shared_ptr<Entity> entity, Camera& camera)
	{
		Editor& editor = Project::GetEditor();
		
		if (entity->HasComponent<ModelComponent>())
			{
				ModelComponent& model_component = entity->GetComponent<ModelComponent>();
				std::shared_ptr<Shader> shader = model_component.shader;
				if (shader->name == "Lit" )
				{
					if (!shader->updated_this_frame)
					{
						shader->Use();
						shader->SetMat4("projection", projection);
						shader->SetMat4("view", view);
						shader->SetVec3("view_pos", camera.position);
						shader->SetFloat("far_plane", scenes.at(current_scene_index)->GetShadowMap()->far_plane);
						shader->SetMat4("light_space_matrix", scenes.at(current_scene_index)->GetShadowMap()->light_space_matrix);
						scenes.at(current_scene_index)->UpdateLightSources(*shader);
						scenes.at(current_scene_index)->GetShadowMap()->Draw();

						bool has_emission = false;
						if (model_component.material && model_component.material->HasTexture(TextureType::EMISSION))
							has_emission = true;
						shader->SetBool("is_emissive", has_emission);

						shader->updated_this_frame = true;
					}

					if (model_component.model->IsAnimated() && entity->HasComponent<AnimationComponent>())
					{
						AnimationComponent& animation_component = entity->GetComponent<AnimationComponent>();
						if (!shader->updated_this_frame)
						{
							const std::vector<glm::mat4>& bone_transforms = animation_component.animator->GetBoneTransforms();
							Log::Info("Uploading " + std::to_string(bone_transforms.size()) + " bone transforms");

							// Check first bone transform
							if (!bone_transforms.empty())
							{
								glm::mat4 first = bone_transforms[0];
								Log::Info("First bone: [" + std::to_string(first[0][0]) + ", " + std::to_string(first[1][1]) + ", " + std::to_string(first[2][2]) + ", " + std::to_string(first[3][3]) + "]");
							}
						}
						if (animation_component.animator)
						{
							const std::vector<glm::mat4>& bone_transforms = animation_component.animator->GetBoneTransforms();
							for (size_t i = 0; i < bone_transforms.size() && i < MAX_BONES; i++)
							{
								shader->SetMat4("bone_transforms["+std::to_string(i)+"]", bone_transforms[i]);
							}
							shader->SetBool("is_animated", true);
						}
						else
							shader->SetBool("is_animated", false);
					}
					else
						shader->SetBool("is_animated", false);
				}
				
				shader->SetBool("reverse_normals", false);
				entity->Draw(model_component.shader, *scenes.at(current_scene_index));
			}
	}

	void Renderer::DrawColliders(const glm::mat4& projection, const glm::mat4& view)
	{
		if (!draw_colliders) return;

		std::shared_ptr<Shader> debug_shader = nullptr;
		for (auto& [id, shader] : scenes.at(current_scene_index)->GetShaders())
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

		for (auto& [id, entity] : scenes.at(current_scene_index)->GetEntities())
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
		bool scene_loaded = scenes.at(current_scene_index)->LoadScene(*param_database);
		return scene_loaded || params_loaded;
	}
	bool Renderer::Save()
	{
		bool scene_saved = scenes.at(current_scene_index)->SaveScene(*param_database);
		bool params_saved = param_database->SaveParams(scenes.at(current_scene_index)->GetMaterials());
		return scene_saved || params_saved;
	}

	bool Renderer::AddScene(std::unique_ptr<Scene> scene)
	{
		scenes.push_back(std::move(scene));
		return true;
	}
	bool Renderer::RemoveScene(std::unique_ptr<Scene> scene)
	{
		auto it = std::find(scenes.begin(), scenes.end(), scene);
		if (it == scenes.end()) return false;
		scenes.erase(it);
		return true;
	}
	void Renderer::NextScene(unsigned int scene_index)
	{
		if (scene_index < 0)
			scene_index = 0;

		if (scene_index >= scenes.size())
			scene_index = scenes.size() - 1;

		current_scene_index = scene_index;
		scenes.at(current_scene_index)->LoadScene(*param_database);
	}
}
