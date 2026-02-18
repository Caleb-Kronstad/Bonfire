#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Utility.hpp"
#include "Core/Engine.hpp"
#include "Core/Debug.hpp"

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
		background_color = RgbaToGlmVec4(23, 23, 23);

		param_database = std::make_unique<ParamDatabase>(
			"Data/Params/models.params", "Data/Params/textures.params", "Data/Params/shaders.params",
			"Data/Params/materials.params", "Data/Params/audios.params", "Data/Params/scripts.params");
		
		std::stringstream path_stream;
		path_stream << "Project Path: " << std::filesystem::current_path();
		Log::Info(path_stream.str());

		project_viewport_framebuffer = std::make_unique<Framebuffer>(project_viewport_size.x, project_viewport_size.y);
	}
	void Renderer::OnDetach()
	{
	}

	void Renderer::OnUpdate(const float& delta_time)
	{
		Engine& project = Engine::Instance();
		Window& project_window = project.GetWindow();

		if (project.GetProjectRunState())
			UpdateAnimations(delta_time);

		if (project.GetEditorRunState())
			RenderViewport(delta_time, GetScene().GetCurrentCamera(), *project_viewport_framebuffer, project_viewport_size);
		else
			RenderViewport(delta_time, GetScene().GetCurrentCamera(), *project_viewport_framebuffer, glm::vec2(project_window.GetWidth(), project_window.GetHeight()));
	}

	void Renderer::RenderViewport(const float& delta_time, Camera& camera, Framebuffer& framebuffer, glm::vec2 viewport_size)
	{
		Engine& project = Engine::Instance();
		Window& project_window = project.GetWindow();

		if (project_window.GetWidth() <= 0 || project_window.GetHeight() <= 0)
			return;

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

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = camera.GetProjectionMatrix(viewport_size.x, viewport_size.y);
		view = camera.GetViewMatrix();
		
		if (GetScene().GetDirectionalLight() != nullptr && GetScene().GetDirectionalLight()->enabled)
		{
			GetScene().GetShadowMap()->LoadDirectional(GetScene().GetDirectionalLight()->direction, camera.position);
			GetScene().GetShadowMap()->SetDirectional();

			for (auto& [shadow_entity_id, shadow_entity] : GetScene().GetEntities())
			{
				if (shadow_entity->HasComponent<ModelComponent>())
				{
					ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
					if (model_component.model->casts_shadow)
					{
						shadow_entity->Draw(GetScene().GetShadowMap()->shadow_map_shader, *scenes.at(current_scene_index));
					}
				}
			}

			GetScene().GetShadowMap()->Reset(false);
			glViewport(0, 0, viewport_size.x, viewport_size.y);
		}
		
		bool shadow_rendered = false;
		for (auto& [entity_id, entity] : GetScene().GetEntities())
		{
			if (!shadow_rendered && entity->HasComponent<LightSourceComponent>())
			{
				LightSourceComponent& light_source_component = entity->GetComponent<LightSourceComponent>();
				if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
				{
					GetScene().GetShadowMap()->Load(point_light->position);
					GetScene().GetShadowMap()->Set(point_light->position);

					for (auto& [shadow_entity_id, shadow_entity] : GetScene().GetEntities())
					{
						if (shadow_entity->HasComponent<ModelComponent>())
						{
							ModelComponent& model_component = shadow_entity->GetComponent<ModelComponent>();
							if (model_component.model->casts_shadow)
							{
								shadow_entity->Draw(GetScene().GetShadowMap()->point_shadow_map_shader, *scenes.at(current_scene_index));
							}
						}
					}

					GetScene().GetShadowMap()->Reset(true);
					break;
				}
			}
		}

		if (project.GetEditorRunState())
			framebuffer.Bind();
		
		glViewport(0, 0, viewport_size.x, viewport_size.y);
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = camera.GetProjectionMatrix(viewport_size.x, viewport_size.y);
		view = camera.GetViewMatrix();

		GetScene().GetShadowMap()->updated_this_frame = false;
		for (auto& [shader_id, shader] : GetScene().GetShaders())
			shader->updated_this_frame = false;
		
		for (auto& [entity_id, entity] : GetScene().GetEntities())
		{
			entity->UpdateComponents();
			DrawEntity(entity, camera);
		}

		DrawColliders(projection, view);

		projection = camera.GetProjectionMatrix(viewport_size.x, viewport_size.y);
		view = camera.GetViewMatrix();
		GetScene().GetSkybox()->Draw(view, projection, *GetScene().GetFog());

		if (project.GetEditorRunState())
		{
			framebuffer.Unbind();
			glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());
		}
	}

	void Renderer::RenderModelPreview(std::shared_ptr<Model> model, std::shared_ptr<Material> material, std::shared_ptr<Shader> shader, Framebuffer& framebuffer, glm::vec3 rotation_angle)
	{
	    if (!model || !material) return;

	    AABB model_aabb = model->CalculateAABB();
	    glm::vec3 center = (model_aabb.minimum + model_aabb.maximum) * 0.5f;
	    glm::vec3 size = model_aabb.maximum - model_aabb.minimum;
	    float max_dimension = (glm::max)(size.x, (glm::max)(size.y, size.z));
	    float camera_distance = max_dimension * 2.0f;

	    Camera preview_camera(9999);
	    preview_camera.position = center + glm::vec3(0.0f, max_dimension * 0.3f, camera_distance);
	    preview_camera.LookAt(center);
	    preview_camera.UpdateCameraVectors();

	    framebuffer.Bind();
	    glViewport(0, 0, 300, 300);
	    glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    glm::mat4 p = preview_camera.GetProjectionMatrix(300, 300);
	    glm::mat4 v = preview_camera.GetViewMatrix();

		shader->Use();
		shader->SetMat4("projection", p);
		shader->SetMat4("view", v);
        shader->SetVec3("view_pos", preview_camera.position);
        if (shader->name == "Lit")
        {
        	shader->SetBool("is_animated", false);
        	shader->SetBool("is_emissive", false);

        	GetScene().GetFog()->ApplyToShader(*shader);
        	GetScene().UpdateLightSources(*shader);
        }

		glm::mat4 model_matrix = glm::mat4(1.0f);
		model_matrix = glm::translate(model_matrix, center);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotation_angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotation_angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotation_angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model_matrix = glm::translate(model_matrix, -center);

		shader->SetMat4("model", model_matrix);
		model->Draw(*shader, material);

	    GetScene().GetSkybox()->Draw(v, p, *GetScene().GetFog());

	    framebuffer.Unbind();
	}

	void Renderer::DrawEntity(std::shared_ptr<Entity> entity, Camera& camera)
	{
		if (entity->HasComponent<ModelComponent>())
		{
			ModelComponent& model_component = entity->GetComponent<ModelComponent>();
			std::shared_ptr<Shader> shader = model_component.shader;
		
			shader->Use();
			if (shader->name == "Lit" || shader->name == "Lit PSX")
			{
				if (!shader->updated_this_frame)
				{
					shader->SetMat4("projection", projection);
					shader->SetMat4("view", view);
					shader->SetVec3("view_pos", camera.position);
					shader->SetFloat("far_plane", GetScene().GetShadowMap()->far_plane);
					shader->SetMat4("light_space_matrix", GetScene().GetShadowMap()->light_space_matrix);
					GetScene().UpdateLightSources(*shader);
					GetScene().GetShadowMap()->Draw();

					GetScene().GetFog()->ApplyToShader(*shader);

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

				bool has_emission = false;
				if (model_component.material && model_component.material->HasTexture(TextureType::EMISSION))
					has_emission = true;
				shader->SetBool("is_emissive", has_emission);
			}
			else if (shader->name == "Water")
			{
				if (!shader->updated_this_frame)
				{
					shader->SetMat4("projection", projection);
					shader->SetMat4("view", view);
					shader->SetVec3("view_pos", camera.position);
					shader->SetFloat("time", glfwGetTime());
					shader->SetVec3("water_color", glm::vec3(1.0f, 1.0f, 1.0f));
					shader->SetFloat("wave_amplitude", 0.3f);
					shader->SetFloat("wave_frequency", 4.0f);
					// maybe add light space matrix for caustics
					shader->updated_this_frame = true;
				}
			}
			
			shader->SetBool("reverse_normals", false);
			entity->Draw(model_component.shader, *scenes.at(current_scene_index));
		}
	}

	void Renderer::DrawColliders(const glm::mat4& projection, const glm::mat4& view)
	{
		if (!draw_colliders) return;

		std::shared_ptr<Shader> debug_shader = nullptr;
		for (auto& [id, shader] : GetScene().GetShaders())
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

		{
			auto physics_lock = Engine::GetThreadManager().LockPhysicsMutex();

			for (auto& [id, entity] : GetScene().GetEntities())
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
				case PhysicsShapeType::MESH:
					if (draw_mesh_colliders && entity->HasComponent<ModelComponent>())
						vertices = Debug::GetMeshVertices(entity->GetComponent<ModelComponent>().model, entity->scale);
					break;
				}

				if (!vertices.empty())
				{
					glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
					glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
					glDrawArrays(GL_LINES, 0, vertices.size());
				}
			}
		}

		glLineWidth(1.0f);
		glEnable(GL_DEPTH_TEST);
		
		glDeleteVertexArrays(1, &vertex_array);
		glDeleteBuffers(1, &vertex_buffer);
	}

	void Renderer::UpdateAnimations(const float& delta_time)
	{
		for (auto& [entity_id, entity] : GetScene().GetEntities())
		{
			if (entity->HasComponent<AnimationComponent>())
			{
				AnimationComponent& animation_component = entity->GetComponent<AnimationComponent>();
				if (animation_component.animator)
					animation_component.animator->Update(delta_time);
			}
		}
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
	
	void Renderer::LoadScene(int scene_index, const std::string& json_data)
	{
		Engine& project = Engine::Instance();
		
		scene_transition_in_progress = true;

		scene_index = (std::max)(0, scene_index);
		scene_index = !scenes.empty() ? (std::min)(scene_index, static_cast<int>(scenes.size()) - 1) : 0;
		GetScene().loaded = false;

		if (project.GetProjectRunState())
		{
			for (auto& [id, animation_component] : GetScene().GetAnimationComponents())
			{
				if (animation_component->animator)
					animation_component->animator->Stop();
			}
			for (auto& [id, audio_component] : GetScene().GetAudioComponents())
			{
				if (audio_component->audio)
					audio_component->audio->Stop();
			}
			for (auto& [id, physics_component] : GetScene().GetPhysicsComponents())
			{
				if (physics_component->physics_body)
					physics_component->physics_body->SetEnabled(false);
			}
		}
		
		current_scene_index = scene_index;
		GetScene().loaded = false;
		json_data.empty() ? GetScene().LoadScene(*param_database) : GetScene().DeserializeFromString(json_data, GetParamDatabase());
		
		for (auto& [id, physics_component] : GetScene().GetPhysicsComponents())
		{
			if (physics_component->physics_body)
				physics_component->physics_body->SetEnabled(false);
		}
		
		for (auto& [entity_id, entity] : GetScene().GetEntities())
		{
			if (entity->HasComponent<ModelComponent>() && entity->HasComponent<PhysicsComponent>())
			{
				ModelComponent& model_component = entity->GetComponent<ModelComponent>();
				PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
				if (physics_component.physics_body->GetShapeData().type == PhysicsShapeType::MESH)
				{
					physics_component.physics_body->SetScale(entity->scale, model_component.model);
				}
			}
		}

		if (project.GetProjectRunState())
		{
			for (auto& [id, audio_component] : GetScene().GetAudioComponents())
			{
				if (audio_component->audio && audio_component->audio->GetPlayOnAwake())
					audio_component->audio->Play();
			}
			for (auto& [id, physics_component] : GetScene().GetPhysicsComponents())
			{
				if (physics_component->physics_body)
					physics_component->physics_body->SetEnabled(true);
			}
		}
		
		GetScene().loaded = true;
		scene_transition_in_progress = false;
	}
}