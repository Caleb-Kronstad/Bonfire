#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
	Renderer::Renderer()
	{
		project_path = std::filesystem::current_path().generic_string();
	}
	Renderer::~Renderer()
	{
		
	}

	void Renderer::OnAttach()
	{
		Project& project = Project::GetInstance();
		Interface& project_interface = project.GetInterface();
		Window& project_window = project.GetWindow();
		
		console_capture = std::make_unique<ConsoleCapture>();
		console_capture->StartCapture();
		
		manipulation_matrix = glm::mat4(1.0f);
		engine_camera_can_rotate = false;

		param_database = std::make_unique<ParamDatabase>("Data/Params/models.params", "Data/Params/textures.params", "Data/Params/shaders.params", "Data/Params/materials.params");
		new_model_path = "Data/Resources/Models/Cube.obj";
		new_texture_path = "Data/Resources/Textures/Checkered.png";
		new_shader_vert_path = "Data/Resources/Shaders/unlit.vert";
		new_shader_frag_path = "Data/Resources/Shaders/unlit.frag";
		new_shader_geom_path = "Data/Resources/Shaders/unlit.geom";

		play_icon = std::make_unique<Texture>("Data/Resources/Textures/Engine/play-icon.png", TextureType::DIFFUSE, false);
		move_icon = std::make_unique<Texture>("Data/Resources/Textures/Engine/move-icon.png", TextureType::DIFFUSE, false);
		rotate_icon = std::make_unique<Texture>("Data/Resources/Textures/Engine/rotate-icon.png", TextureType::DIFFUSE, false);
		resize_icon = std::make_unique<Texture>("Data/Resources/Textures/Engine/resize-icon.png", TextureType::DIFFUSE, false);
		play_icon->Load();
		move_icon->Load();
		rotate_icon->Load();
		resize_icon->Load();

		background_color = RgbaToGlmVec4(project_interface.background_secondary.x, project_interface.background_secondary.y, project_interface.background_secondary.z);
		viewport_framebuffer = std::make_unique<Framebuffer>(viewport_size.x, viewport_size.y);
		
		std::stringstream path_stream;
		path_stream << "Project Path: " << std::filesystem::current_path();
		Log::Info(path_stream.str());

		// LOAD FONTS
		ImGuiIO& io = ImGui::GetIO();
		font_title = io.Fonts->AddFontFromFileTTF("Data/Resources/Fonts/Space_Mono/SpaceMono-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		font_body = io.Fonts->AddFontFromFileTTF("Data/Resources/Fonts/Space_Mono/SpaceMono-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		
		// SCENE AND EDITOR LOADING
		scene = std::make_unique<Scene>("Data/Scenes/testscene.bonfirescene");
		Load();
		for (auto& [shader_id, shader] : scene->GetShaders())
		{
			shader->Use();
			shader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));
		}
	}
	void Renderer::OnDetach()
	{
		console_capture->StopCapture();
	}

	void Renderer::OnUpdate()
	{
		Project& project = Project::GetInstance();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Window& project_window = project.GetWindow();
		GLFWwindow* glfw_window = project_window.GetNativeWindow();
		
		if (project.GetProjectRunState())
		{
			if (physics_system.paused) 
				physics_system.paused = false;
		}
		else
		{
			if (!physics_system.paused)
				physics_system.paused = true;
		}
		
		const float deltaTime = project.GetDeltaTime();

		// --- TESTING - IMPROVE IMPLEMENTATION AT LATER TIME ---
		if (project_window.GetWidth() <= 0 || project_window.GetHeight() <= 0)
			return;
		if (viewport_focused)
		{
			if (glfwGetKey(glfw_window, InputCode::W) == GLFW_PRESS)
				scene->GetEngineCamera()->ProcessKeyboard(MovementDirection::FORWARD, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::S) == GLFW_PRESS)
				scene->GetEngineCamera()->ProcessKeyboard(MovementDirection::BACKWARD, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::A) == GLFW_PRESS)
				scene->GetEngineCamera()->ProcessKeyboard(MovementDirection::LEFT, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::D) == GLFW_PRESS)
				scene->GetEngineCamera()->ProcessKeyboard(MovementDirection::	RIGHT, deltaTime);
		}
		// ---

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = scene->GetEngineCamera()->GetProjectionMatrix(viewport_size.x, viewport_size.y);
		glm::mat4 view = scene->GetEngineCamera()->GetViewMatrix();

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
						shadow_entity->Draw(scene->GetShadowMap()->point_shadow_map_shader, *scene, manipulation_matrix, view, projection);
					}

					scene->GetShadowMap()->Reset(true);
					shadow_rendered = true; 
					break;
				}
			}
		}
		
		viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = scene->GetEngineCamera()->GetProjectionMatrix(viewport_size.x, viewport_size.y);
		view = scene->GetEngineCamera()->GetViewMatrix();

		scene->GetShadowMap()->updated_this_frame = false;
		for (auto& [shader_id, shader] : scene->GetShaders())
			shader->updated_this_frame = false;
		
		for (auto& [entity_id, entity] : scene->GetEntities())
		{
			if (entity->HasComponent<ModelComponent>())
			{
				ModelComponent& model_component = entity->GetComponent<ModelComponent>();
				entity->Draw(model_component.shader, *scene, manipulation_matrix, view, projection);
			}
			if (entity->HasComponent<PhysicsComponent>() && project.GetProjectRunState())
			{
				PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
				std::shared_ptr<PhysicsBody> physics_body = physics_component.physics_body;
				entity->position = physics_body->GetPosition();
				entity->rotation = glm::degrees(glm::eulerAngles(physics_body->GetRotation()));
			}
		}

		projection = scene->GetEngineCamera()->GetProjectionMatrix(viewport_size.x, viewport_size.y);
		view = scene->GetEngineCamera()->GetViewMatrix();
		scene->GetSkybox()->Draw(view, projection);

		viewport_framebuffer->Unbind();
		glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());
	}

	// Used for engine input, not game logic input
	void Renderer::OnInput(Input& input)
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();
		GLFWwindow* glfw_window = window.GetNativeWindow();

		switch (input.GetInputType())
		{
		case InputType::KeyPressed:
			{
				const auto keyInput = dynamic_cast<KeyPressedInput&>(input);

				// -- actions here --

				// maximize window
				if (keyInput.GetKeyCode() == InputCode::F11)
				{
					glfwMaximizeWindow(window.GetNativeWindow());
				}
				
				break;
			}
		case InputType::KeyReleased:
			{
				const auto keyInput = dynamic_cast<KeyReleasedInput&>(input);

				// -- actions here --
				
				break;
			}
		case InputType::KeyTyped:
			{
				const auto keyInput = dynamic_cast<KeyTypedInput&>(input);

				// -- actions here --

				break;
			}
		case InputType::MouseButtonPressed:
			{
				const auto mouseInput = dynamic_cast<MouseButtonPressedInput&>(input);
				
				// -- actions here --

				// enable engine camera rotation
				if (mouseInput.GetMouseButton() == InputCode::Button1)
				{
					if (viewport_focused)
					{
						engine_camera_can_rotate = true;
						glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					}
				}
				
				break;
			}
		case InputType::MouseButtonReleased:
			{
				const auto mouseInput = dynamic_cast<MouseButtonReleasedInput&>(input);
				
				// -- actions here --

				// disable engine camera rotation
				if (mouseInput.GetMouseButton() == InputCode::Button1)
				{
					engine_camera_can_rotate = false;
					glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				
				break;
			}
		case InputType::MouseMoved:
			{
				const auto mouseInput = dynamic_cast<MouseMovedInput&>(input);

				// -- actions here --

				// move engine camera based on mouse position and movement
				const float x_position = mouseInput.GetX();
				const float y_position = mouseInput.GetY();

				if (scene->GetEngineCamera()->firstMouse)
				{
					scene->GetEngineCamera()->lastX = x_position;
					scene->GetEngineCamera()->lastY = y_position;
					scene->GetEngineCamera()->firstMouse = false;
				}

				const float x_offset = x_position - scene->GetEngineCamera()->lastX;
				const float y_offset = y_position - scene->GetEngineCamera()->lastY;

				scene->GetEngineCamera()->lastX = x_position;
				scene->GetEngineCamera()->lastY = y_position;

				if (engine_camera_can_rotate)
					scene->GetEngineCamera()->ProcessMouseMovement(x_offset, y_offset);
				
				break;
			}
		case InputType::MouseScrolled:
			{
				const auto mouseInput = dynamic_cast<MouseScrolledInput&>(input);

				// -- actions here --
				
				break;
			}
		case InputType::None:
				break;
		}
	}

	void Renderer::CreateEntity(std::shared_ptr<Entity> parent)
	{
		uint32_t next_id = 1000001;
		if (!scene->GetEntities().empty())
		{
			auto max_it = std::max_element(
			  scene->GetEntities().begin(),
			  scene->GetEntities().end(),
			  [](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}
		std::shared_ptr<Entity> new_entity = std::make_shared<Entity>(next_id);
		if (parent != nullptr)
		{
			new_entity->parent = parent->id;
			scene->GetEntities().at(parent->id)->AddChild(new_entity->id);
		}
		scene->GetEntities().insert_or_assign(next_id, new_entity);
	}
	void Renderer::DuplicateEntity(std::shared_ptr<Entity> entity)
	{
	    std::function<uint32_t(std::shared_ptr<Entity>)> DuplicateRecursive;
		DuplicateRecursive = [&](std::shared_ptr<Entity> ent) -> uint32_t
		{
			uint32_t next_entity_id = 1000001;
			if (!scene->GetEntities().empty())
			{
				auto max_it = std::max_element(
				  scene->GetEntities().begin(),
				  scene->GetEntities().end(),
				  [](const auto& a, const auto& b) { return a.first < b.first; }
				);
				next_entity_id = max_it->first + 1;
			}

			std::shared_ptr<Entity> duplicated = std::make_shared<Entity>(*ent);
			duplicated->id = next_entity_id;
			std::vector<uint32_t> original_children = duplicated->children;
			duplicated->children.clear();

			if (ent->HasComponent<ModelComponent>())
			{
				auto& original_component = ent->GetComponent<ModelComponent>();

				uint32_t next_comp_id = 100001;
				if (!scene->GetModelComponents().empty())
				{
				  auto max_comp = std::max_element(
				      scene->GetModelComponents().begin(),
				      scene->GetModelComponents().end(),
				      [](const auto& a, const auto& b) { return a.first < b.first; }
				  );
				  next_comp_id = max_comp->first + 1;
				}

				std::shared_ptr<ModelComponent> new_component = std::make_shared<ModelComponent>(
				  next_comp_id,
				  original_component.enabled,
				  original_component.model,
				  original_component.shader,
				  original_component.material
				);

				scene->GetModelComponents().insert_or_assign(next_comp_id, new_component);
				duplicated->RemoveComponent(ComponentType::MODEL);
				duplicated->AddComponent(ComponentType::MODEL, new_component);
			}
			if (ent->HasComponent<LightSourceComponent>())
            {
                auto& original_component = ent->GetComponent<LightSourceComponent>();

                // Generate new component ID
                uint32_t next_comp_id = 100001;
                if (!scene->GetLightSourceComponents().empty())
                {
                        auto max_comp = std::max_element(
                                scene->GetLightSourceComponents().begin(),
                                scene->GetLightSourceComponents().end(),
                                [](const auto& a, const auto& b) { return a.first < b.first; }
                        );
                        next_comp_id = max_comp->first + 1;
                }

                uint32_t next_light_id = 1000;

                std::shared_ptr<LightSource> new_light_source;

                if (auto point_light = std::dynamic_pointer_cast<PointLight>(original_component.light_source))
                {
                        if (!scene->GetPointLights().empty())
                        {
                                auto max_light = std::max_element(
                                        scene->GetPointLights().begin(),
                                        scene->GetPointLights().end(),
                                        [](const auto& a, const auto& b) { return a.first < b.first; }
                                );
                                next_light_id = max_light->first + 1;
                        }

                        std::shared_ptr<PointLight> new_point_light = std::make_shared<PointLight>();
                        new_point_light->id = next_light_id;
                        new_point_light->enabled = point_light->enabled;
                        new_point_light->position = point_light->position;
                        new_point_light->color = point_light->color;
                        new_point_light->scale = point_light->scale;

                        new_light_source = new_point_light;
                        scene->GetPointLights().insert_or_assign(next_light_id, new_point_light);
                }
                else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(original_component.light_source))
                {
                        if (!scene->GetSpotLights().empty())
                        {
                                auto max_light = std::max_element(
                                        scene->GetSpotLights().begin(),
                                        scene->GetSpotLights().end(),
                                        [](const auto& a, const auto& b) { return a.first < b.first; }
                                );
                                next_light_id = max_light->first + 1;
                        }

                        std::shared_ptr<SpotLight> new_spot_light = std::make_shared<SpotLight>();
                        new_spot_light->id = next_light_id;
                        new_spot_light->enabled = spot_light->enabled;
                        new_spot_light->position = spot_light->position;
                        new_spot_light->color = spot_light->color;
                        new_spot_light->scale = spot_light->scale;
                        new_spot_light->direction = spot_light->direction;

                        new_light_source = new_spot_light;
                        scene->GetSpotLights().insert_or_assign(next_light_id, new_spot_light);
                }

                std::shared_ptr<LightSourceComponent> new_component = std::make_shared<LightSourceComponent>(
                        next_comp_id,
                        original_component.enabled,
                        new_light_source
                );

                scene->GetLightSourceComponents().insert_or_assign(next_comp_id, new_component);
                duplicated->RemoveComponent(ComponentType::LIGHT);
                duplicated->AddComponent(ComponentType::LIGHT, new_component);
            }
			if (ent->HasComponent<PhysicsComponent>())
		    {
		        PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		        auto& original_component = ent->GetComponent<PhysicsComponent>();
  
		        uint32_t next_comp_id = 100001;
		        if (!scene->GetPhysicsComponents().empty())
		        {
		            auto max_comp = std::max_element(
		                scene->GetPhysicsComponents().begin(),
		                scene->GetPhysicsComponents().end(),
		                [](const auto& a, const auto& b) { return a.first < b.first; }
		            );
		            next_comp_id = max_comp->first + 1;
		        }

		        uint32_t next_physics_id = 1000;
		        if (!scene->GetPhysicsComponents().empty())
		        {
		            uint32_t max_physics_id = 1000;
		            for (const auto& [id, comp] : scene->GetPhysicsComponents())
		            {
		                if (comp->physics_body && comp->physics_body->id > max_physics_id)
		                    max_physics_id = comp->physics_body->id;
		            }
		            next_physics_id = max_physics_id + 1;
		        }

		        PhysicsBodyType body_type = original_component.physics_body->GetBodyType();
		        PhysicsShapeData shape_data = original_component.physics_body->GetShapeData();
		        glm::vec3 position = original_component.physics_body->GetPosition();
		        glm::quat rotation = original_component.physics_body->GetRotation();
		        bool enabled = original_component.physics_body->enabled;
		        std::string name = original_component.physics_body->name;

		        std::shared_ptr<PhysicsBody> new_physics_body;

		        if (shape_data.type == PhysicsShapeType::BOX)
		            new_physics_body = physics_system.CreateBoxBody(position, rotation, shape_data.dimensions, body_type);
		        else if (shape_data.type == PhysicsShapeType::SPHERE)
		            new_physics_body = physics_system.CreateSphereBody(position, shape_data.dimensions.x, body_type);
		        else if (shape_data.type == PhysicsShapeType::CAPSULE)
		            new_physics_body = physics_system.CreateCapsuleBody(position, rotation, shape_data.dimensions.x, shape_data.dimensions.y, body_type);

		        if (new_physics_body)
		        {
		            new_physics_body->id = next_physics_id;
		            new_physics_body->enabled = enabled;
		            new_physics_body->SetEnabled(enabled);
		            new_physics_body->name = name;

		            std::shared_ptr<PhysicsComponent> new_component = std::make_shared<PhysicsComponent>(
		                next_comp_id,
		                original_component.enabled,
		                new_physics_body
		                );

		            scene->GetPhysicsComponents().insert_or_assign(next_comp_id, new_component);
		            duplicated->RemoveComponent(ComponentType::PHYSICS);
		            duplicated->AddComponent(ComponentType::PHYSICS, new_component);
		        }
		    }
			
		    scene->GetEntities().insert_or_assign(next_entity_id, duplicated);

		    for (uint32_t child_id : original_children)
		    {
			    if (scene->GetEntities().contains(child_id))
			    {
				    uint32_t new_child_id = DuplicateRecursive(scene->GetEntities().at(child_id));
				    duplicated->children.push_back(new_child_id);
				    scene->GetEntities().at(new_child_id)->parent = next_entity_id;
			    }
		    }

		    return next_entity_id;
		};

		uint32_t original_parent = entity->parent;
		bool original_is_root = entity->IsRoot();
		uint32_t new_root_id = DuplicateRecursive(entity);

		scene->GetEntities().at(new_root_id)->parent = original_parent;

		if (!original_is_root && scene->GetEntities().contains(original_parent))
			scene->GetEntities().at(original_parent)->children.push_back(new_root_id);

		selected_entity = scene->GetEntities().at(new_root_id);
	}

	void Renderer::DeleteEntity(std::shared_ptr<Entity> entity)
	{
		std::function<void(std::shared_ptr<Entity>)> DeleteRecursive;
		DeleteRecursive = [&](std::shared_ptr<Entity> ent)
		{
			std::vector<uint32_t> children_copy = ent->children;
			for (uint32_t child_id : children_copy)
			{
				if (scene->GetEntities().contains(child_id))
				{
					DeleteRecursive(scene->GetEntities()[child_id]);
				}
			}

			if (ent->HasComponent<ModelComponent>())
			{
				auto& model_component = ent->GetComponent<ModelComponent>();
				scene->GetModelComponents().erase(model_component.id);
			}
			if (ent->HasComponent<LightSourceComponent>())
			{
				auto& light_source_component = ent->GetComponent<LightSourceComponent>();
				if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
				{
					scene->GetPointLights().erase(point_light->id);
				}
				if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_source_component.light_source))
				{
					scene->GetSpotLights().erase(spot_light->id);
				}
				scene->GetLightSourceComponents().erase(light_source_component.id);
			}
			if (ent->HasComponent<PhysicsComponent>())
			{
				auto& physics_component = ent->GetComponent<PhysicsComponent>();
				PhysicsSystem& physics_system = Project::GetPhysicsSystem();
				JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
    
				if (physics_component.physics_body != nullptr)
				{
					JPH::BodyID body_id = physics_component.physics_body->GetBodyID();
					if (body_interface.IsAdded(body_id))
					{
						body_interface.RemoveBody(body_id);
						body_interface.DestroyBody(body_id);
					}
				}
    
				scene->GetPhysicsComponents().erase(physics_component.id);
			}
			if (ent->HasComponent<AnimationComponent>())
			{
				auto& animation_component = ent->GetComponent<AnimationComponent>();
				// delete animation component
			}

			if (selected_entity == ent)
				selected_entity = nullptr;

			scene->GetEntities().erase(ent->id);
		};

		if (!entity->IsRoot() && scene->GetEntities().contains(entity->parent))
		{
			scene->GetEntities()[entity->parent]->RemoveChild(entity->id);
		}

		DeleteRecursive(entity);

		if (selected_entity == nullptr && !scene->GetEntities().empty())
			selected_entity = scene->GetEntities().begin()->second;
	}
	
	bool Renderer::IsDescendentOf(std::shared_ptr<Entity> potential_child, std::shared_ptr<Entity> potential_parent)
	{
		if (potential_parent == nullptr)
			return false;
		if (potential_parent == potential_child)
			return true;

		for (uint32_t child_id : potential_child->children)
		{
			if (scene->GetEntities().contains(child_id))
			{
				if (IsDescendentOf(scene->GetEntities().at(child_id), potential_parent))
					return true;
			}
		}

		return false;
	}
	void Renderer::ReparentEntity(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> new_parent)
	{
		glm::mat4 world_transform = entity->GetWorldTransformMatrix(scene->GetEntities());
		
		if (!entity->IsRoot() && scene->GetEntities().contains(entity->parent))
			scene->GetEntities().at(entity->parent)->RemoveChild(entity->id);

		if (new_parent != nullptr)
		{
			entity->parent = new_parent->id;
			new_parent->AddChild(entity->id);

			glm::mat4 parent_world_transform = new_parent->GetWorldTransformMatrix(scene->GetEntities());
			glm::mat4 new_local_transform = glm::inverse(parent_world_transform) * world_transform;

			glm::vec3 translation, rotation, scale;
			DecomposeTransform(new_local_transform, translation, rotation, scale);
			entity->position = translation;
			entity->rotation = glm::degrees(rotation);
			entity->scale = scale;
		}
		else
		{
			entity->parent = 0;

			glm::vec3 translation, rotation, scale;
			DecomposeTransform(world_transform, translation, rotation, scale);
			entity->position = translation;
			entity->rotation = glm::degrees(rotation);
			entity->scale = scale;
		}
	}

	bool Renderer::Load()
	{
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		bool params_loaded = param_database->LoadParams();
		bool scene_loaded = scene->LoadScene(*param_database);
		selected_entity = nullptr;
		return scene_loaded || params_loaded;
	}
	bool Renderer::Save()
	{
		bool scene_saved = scene->SaveScene(*param_database);
		bool params_saved = param_database->SaveParams(scene->GetMaterials());
		return scene_saved || params_saved;
	}
	
	void Renderer::DrawActiveTitleLine(const ImVec4& active_color, const ImVec4& inactive_color, float thickness)
	{
		ImVec4 color = active_color;
		if (!ImGui::IsWindowFocused())
			color = inactive_color;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();

		if (window->DockNode && window->DockNode->TabBar)
		{
			ImGuiTabBar* tab_bar = window->DockNode->TabBar;
			ImGuiTabItem* tab = ImGui::TabBarFindTabByID(tab_bar, window->TabId);

			if (tab)
			{
				float tab_x = tab_bar->BarRect.Min.x + tab->Offset;
				float tab_y = tab_bar->BarRect.Min.y;
				float tab_width = tab->Width;

				draw_list->AddRectFilled(
					ImVec2(tab_x, tab_y),
					ImVec2(tab_x + tab_width, tab_y + thickness),
					ImGui::ColorConvertFloat4ToU32(color)
				);
				return;
			}
		}

		ImVec2 window_pos = ImGui::GetWindowPos();
		const char* title = window->Name;
		ImVec2 text_size = ImGui::CalcTextSize(title);
		float title_padding = ImGui::GetStyle().FramePadding.x;
		float title_x = window_pos.x + title_padding * 3.5f;

		draw_list->AddRectFilled(
			ImVec2(window_pos.x + title_padding * 1.25f, window_pos.y),
			ImVec2(title_x + text_size.x, window_pos.y + thickness),
			ImGui::ColorConvertFloat4ToU32(color)
		);
	}
}
