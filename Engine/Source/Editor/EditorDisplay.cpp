#include "bonfire_pch.hpp"
#include "Editor.hpp"

#include "Commands.hpp"
#include "Core/Utility.hpp"
#include "Core/Project.hpp"

namespace Bonfire
{
    void Editor::DisplayModelComponent()
    {
    	Scene& scene = Project::GetRenderer().GetScene();
    	
        if (selected_entity->HasComponent<ModelComponent>())
    	{
    		ModelComponent& model_component = selected_entity->GetComponent<ModelComponent>();
    		
    		ImGui::PushID(&model_component);
    		ImGui::Separator();
    		
    		ImGui::Checkbox("##Enabled", &model_component.enabled);
    		ImGui::SameLine();
    		ImGui::Text("Model Component");
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(model_component.id).c_str());
    		ImGui::Spacing();

    		ImGui::Checkbox("Casts Shadow", &model_component.model->casts_shadow);

    		if (ImGui::Button(model_component.model->name.c_str(), ImVec2(100, 22)))
    			ImGui::OpenPopup("ChangeModelModelComponent");
    		ImGui::SameLine(); ImGui::Text("Model");
    		if (ImGui::Button(model_component.shader->name.c_str(), ImVec2(100, 22)))
    			ImGui::OpenPopup("ChangeShaderModelComponent");
    		ImGui::SameLine(); ImGui::Text("Shader");
    		if (ImGui::Button(model_component.material->name.c_str(), ImVec2(100, 22)))
    			ImGui::OpenPopup("ChangeMaterialModelComponent");
    		ImGui::SameLine(); ImGui::Text("Material");

    		if (ImGui::BeginPopup("ChangeModelModelComponent"))
    		{
    			for (auto& [id, scene_item] : scene.GetModels())
    			{
    				ImGui::PushID(&id);
    				if (ImGui::Selectable(scene_item->name.c_str(), false, 0))
    				{
    					model_component.model = scene_item;
    					ImGui::CloseCurrentPopup();
    				}
    				ImGui::PopID();
    			}
    			ImGui::EndPopup();
    		}
    		if (ImGui::BeginPopup("ChangeShaderModelComponent"))
    		{
    			for (auto& [id, scene_item] : scene.GetShaders())
    			{
    				ImGui::PushID(&id);
    				if (ImGui::Selectable(scene_item->name.c_str(), false, 0))
    				{
    					model_component.shader = scene_item;
    					ImGui::CloseCurrentPopup();
    				}
    				ImGui::PopID();
    			}
    			ImGui::EndPopup();
    		}
    		if (ImGui::BeginPopup("ChangeMaterialModelComponent"))
    		{
    			for (auto& [id, scene_item] : scene.GetMaterials())
    			{
    				ImGui::PushID(&id);
    				if (ImGui::Selectable(scene_item->name.c_str(), false, 0))
    				{
    					model_component.material = scene_item;
    					ImGui::CloseCurrentPopup();
    				}
    				ImGui::PopID();
    			}
    			ImGui::EndPopup();
    		}

    		ImGui::PopID();
    	}
    }
	
    void Editor::DisplayLightSourceComponent()
    {
    	Scene& scene = Project::GetRenderer().GetScene();

    	if (selected_entity->HasComponent<LightSourceComponent>())
    	{
    		LightSourceComponent& light_source_component = selected_entity->GetComponent<LightSourceComponent>();
    		
    		ImGui::PushID(&light_source_component);
    		ImGui::Separator();

    		ImGui::Checkbox("##Enabled", &light_source_component.enabled);
    		ImGui::SameLine();
    		ImGui::Text("Light Source Component");
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(light_source_component.id).c_str());
    		ImGui::Spacing();
    		
    		if (ImGui::Button(light_source_component.light_source->name.c_str(), ImVec2(100, 22)))
    			ImGui::OpenPopup("ChangeLightSourceComponentLightSource");
    		ImGui::SameLine(); ImGui::Text("Light Source Type");

    		if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
    		{
    			ImGui::Spacing();
    			ImGui::Text("Point Light Properties");
    			ImGui::PushItemWidth(200.0f);
    			ImGui::SliderFloat("Intensity", &point_light->intensity, 0.0f, 10.0f, "%.1f");
    			ImGui::SliderFloat3("Color", (float*)&point_light->color, 0.0f, 255.0f, "%1.f");
    			ImGui::PopItemWidth();
    		}
    		else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_source_component.light_source))
    		{
    			ImGui::Spacing();
    			ImGui::Text("Spot Light Properties");
    			ImGui::PushItemWidth(200.0f);
    			ImGui::SliderFloat3("Color", (float*)&spot_light->color, 0.0f, 255.0f, "%1.f");
    			ImGui::DragFloat3("Scale", (float*)&spot_light->scale, drag_step, 0.0f, 100.0f, "%.1f");
    			ImGui::DragFloat3("Direction", (float*)&spot_light->direction, drag_step, -1.0f, 1.0f, "%.2f");
    			ImGui::PopItemWidth();
    		}

    		if (ImGui::BeginPopup("ChangeLightSourceComponentLightSource"))
    		{
    			auto old_light = light_source_component.light_source;
    			glm::vec3 preserved_color = glm::vec3(255.0f);
    			glm::vec3 preserved_position = glm::vec3(0.0f);
    			glm::vec3 preserved_scale = glm::vec3(1.0f);
    			glm::vec3 preserved_direction = glm::vec3(0.0f);
    			float preserved_intensity = 1.0f;

    			if (auto point_light = std::dynamic_pointer_cast<PointLight>(old_light))
    			{
    				preserved_color = point_light->color;
    				preserved_position = point_light->position;
    				preserved_scale = point_light->scale;
    				preserved_intensity = point_light->intensity;
    			}
    			else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(old_light))
    			{
    				preserved_color = spot_light->color;
    				preserved_position = spot_light->position;
    				preserved_scale = spot_light->scale;
    				preserved_direction = spot_light->direction;
    			}
    			
    			if (ImGui::MenuItem("Point Light"))
    			{
    				if (auto current_type = std::dynamic_pointer_cast<PointLight>(old_light))
    					ImGui::CloseCurrentPopup();
    				if (auto old_spot = std::dynamic_pointer_cast<SpotLight>(old_light))
    					scene.GetSpotLights().erase(old_spot->id);

    				std::shared_ptr<PointLight> new_light = std::make_shared<PointLight>();
    				new_light->color = preserved_color;
    				new_light->position = preserved_position;
    				new_light->scale = preserved_scale;
    				new_light->intensity = preserved_intensity;
    				new_light->id = old_light->id;
    				new_light->enabled = old_light->enabled;
    				light_source_component.light_source = new_light;

    				scene.GetPointLights().insert_or_assign(new_light->id, new_light);
    				ImGui::CloseCurrentPopup();
    			}
    			if (ImGui::MenuItem("Spot Light"))
    			{
    				if (auto current_type = std::dynamic_pointer_cast<SpotLight>(old_light))
    					ImGui::CloseCurrentPopup();
    				if (auto old_point = std::dynamic_pointer_cast<PointLight>(old_light))
    					scene.GetPointLights().erase(old_point->id);

    				std::shared_ptr<SpotLight> new_light = std::make_shared<SpotLight>();
    				new_light->color = preserved_color;
    				new_light->position = preserved_position;
    				new_light->scale = preserved_scale;
    				new_light->direction = preserved_direction;
    				new_light->id = old_light->id;
    				new_light->enabled = old_light->enabled;
    				light_source_component.light_source = new_light;

    				scene.GetSpotLights().insert_or_assign(new_light->id, new_light);
    				ImGui::CloseCurrentPopup();
    			}
    			ImGui::EndPopup();
    		}
    		
    		ImGui::PopID();
    	}
    }
	
    void Editor::DisplayPhysicsComponent()
    {
    	PhysicsSystem& physics_system = Project::GetPhysicsSystem();
    	Scene& scene = Project::GetRenderer().GetScene();
    	
        if (selected_entity->HasComponent<PhysicsComponent>())
    	{
    		PhysicsComponent& physics_component = selected_entity->GetComponent<PhysicsComponent>();

    		ImGui::PushID(&physics_component);
    		ImGui::Separator();
			
    		ImGui::Checkbox("##Enabled", &physics_component.enabled);
    		ImGui::SameLine();
    		ImGui::Text("Physics Component");
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(physics_component.id).c_str());
    		ImGui::Spacing();
			
    		int selected_body_type = static_cast<int>(physics_component.physics_body->GetBodyType());
    		int selected_shape_type = static_cast<int>(physics_component.physics_body->GetShapeData().type);
    		PhysicsShapeData current_shape_data = physics_component.physics_body->GetShapeData();

    		const char* body_type_names[] = { "STATIC", "DYNAMIC", "KINEMATIC" };
    		const char* shape_type_names[] = { "BOX", "SPHERE", "CAPSULE", "MESH" };

    		bool body_type_changed = ImGui::Combo("Body Type", &selected_body_type, body_type_names,
			IM_ARRAYSIZE(body_type_names));
    		bool shape_type_changed = ImGui::Combo("Shape Type", &selected_shape_type, shape_type_names,
			IM_ARRAYSIZE(shape_type_names));

    		if (body_type_changed || shape_type_changed)
    		{
    			PhysicsBodyType new_body_type = static_cast<PhysicsBodyType>(selected_body_type);
    			PhysicsShapeType new_shape_type = static_cast<PhysicsShapeType>(selected_shape_type);

    			glm::vec3 position = physics_component.physics_body->GetPosition();
    			glm::quat rotation = physics_component.physics_body->GetRotation();
    			uint32_t id = physics_component.physics_body->id;
    			bool enabled = physics_component.physics_body->enabled;
    			std::string name = physics_component.physics_body->name;
    			glm::vec3 dimensions = current_shape_data.dimensions;

    			std::shared_ptr<PhysicsBody> new_physics_body;

    			if (new_shape_type == PhysicsShapeType::BOX)
    				new_physics_body = physics_system.CreateBoxBody(position, rotation, dimensions, new_body_type);
    			else if (new_shape_type == PhysicsShapeType::SPHERE)
    				new_physics_body = physics_system.CreateSphereBody(position, dimensions.x, new_body_type);
    			else if (new_shape_type == PhysicsShapeType::CAPSULE)
    				new_physics_body = physics_system.CreateCapsuleBody(position, rotation, dimensions.x, dimensions.y, new_body_type);
    			else if (new_shape_type == PhysicsShapeType::MESH)
    			{
    				if (selected_entity->HasComponent<ModelComponent>())
    				{
    					ModelComponent& model_component = selected_entity->GetComponent<ModelComponent>();
    					if (model_component.model)
    					{
    						new_physics_body = physics_system.CreateMeshBody(
								position,
								rotation,
								model_component.model,
								model_component.model->param_id,
								PhysicsBodyType::STATIC
							);
    					}
    					else
    					{
    						Log::Error("Cannot create mesh collider: entity has no model");
    						selected_shape_type = static_cast<int>(current_shape_data.type);
    					}
    				}
    				else
    				{
    					Log::Error("Cannot create mesh collider: entity must have a ModelComponent");
    					selected_shape_type = static_cast<int>(current_shape_data.type);
    				}
    			}

    			new_physics_body->id = id;
    			new_physics_body->enabled = enabled;
    			new_physics_body->name = name;
    			new_physics_body->SetEnabled(enabled);

    			physics_component.physics_body = new_physics_body;
    		}

    		PhysicsShapeData shape_data = physics_component.physics_body->GetShapeData();

		    if (shape_data.type == PhysicsShapeType::MESH)
		    {
		    	ImGui::Separator();
		    	ImGui::Text("Vertices: ", static_cast<int>(shape_data.dimensions.x));
		    	ImGui::Text("Triangles: ", static_cast<int>(shape_data.dimensions.y));
		    	ImGui::Text("Model ID: ", static_cast<int>(shape_data.dimensions.z));

		    	if (ImGui::Button("Apply Mesh Size"))
		    	{
		    		glm::mat4 world_transform = selected_entity->GetWorldTransformMatrix(Project::GetRenderer().GetScene().GetEntities());
		    		glm::vec3 world_position, world_rotation, world_scale;
		    		DecomposeTransform(world_transform, world_position, world_rotation, world_scale);

		    		std::shared_ptr<Model> model = nullptr;
		    		if (selected_entity->HasComponent<ModelComponent>())
		    		{
		    			ModelComponent& model_component = selected_entity->GetComponent<ModelComponent>();
		    			model = model_component.model;
		    		}

		    		physics_component.physics_body->SetScale(world_scale, model);
		    	}

		    	ImGui::Spacing();
		    	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Mesh colliders must be STATIC");
		    }
		    else
		    {
		        glm::vec3 dimensions = shape_data.dimensions;
  
		        if (shape_data.type == PhysicsShapeType::BOX)
		        {
		            /*if (ImGui::DragFloat3("Half Extents", (float*)&dimensions, drag_step, 0.1f, 100.0f))
		            {
		                physics_component.physics_body->GetShapeData().dimensions = selected_entity->scale;
		                physics_component.physics_body->SetScale(selected_entity->scale);
		            }*/
		        	physics_component.physics_body->GetShapeData().dimensions = selected_entity->scale;
		        	physics_component.physics_body->SetScale(selected_entity->scale);
		        }
		        else if (shape_data.type == PhysicsShapeType::SPHERE)
		        {
		            if (ImGui::DragFloat("Radius", &dimensions.x, drag_step, 0.1f, 100.0f))
		            {
		                physics_component.physics_body->GetShapeData().dimensions = dimensions;
		                physics_component.physics_body->SetScale(dimensions);
		            }
		        }
		        else if (shape_data.type == PhysicsShapeType::CAPSULE)
		        {
		            if (ImGui::DragFloat("Radius", &dimensions.x, drag_step, 0.1f, 100.0f))
		            {
		                physics_component.physics_body->GetShapeData().dimensions = dimensions;
		                physics_component.physics_body->SetScale(dimensions);
		            }
		            if (ImGui::DragFloat("Half Height", &dimensions.y, drag_step, 0.1f, 100.0f))
		            {
		                physics_component.physics_body->GetShapeData().dimensions = dimensions;
		                physics_component.physics_body->SetScale(dimensions);
		            }
		        }	
		    }

    		ImGui::Spacing();

    		ImGui::Text("Can Move");
    		ImGui::Checkbox("x##p", &physics_component.can_move_axis[0]); ImGui::SameLine(); ImGui::Checkbox("y##p", &physics_component.can_move_axis[1]); ImGui::SameLine(); ImGui::Checkbox("z##p", &physics_component.can_move_axis[2]);
    		ImGui::Text("Can Rotate");
    		ImGui::Checkbox("x##r", &physics_component.can_rotate_axis[0]); ImGui::SameLine(); ImGui::Checkbox("y##r", &physics_component.can_rotate_axis[1]); ImGui::SameLine(); ImGui::Checkbox("z##r", &physics_component.can_rotate_axis[2]);
    		ImGui::Spacing();

    		selected_entity->ValidateDOFS();
    		selected_entity->UpdateComponents();
    		
    		ImGui::PopID();
    	}
    }
	
    void Editor::DisplayAnimationComponent()
    {
    	Scene& scene = Project::GetRenderer().GetScene();
    	
        if (selected_entity->HasComponent<AnimationComponent>())
    	{
			AnimationComponent& animation_component = selected_entity->GetComponent<AnimationComponent>();
    		Animator& animator = *animation_component.animator;

    		ImGui::PushID(&animation_component);
    		ImGui::Separator();
			
    		ImGui::Checkbox("##Enabled", &animation_component.enabled);
    		ImGui::SameLine();
    		ImGui::Text("Animation Component");
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(animation_component.id).c_str());
    		ImGui::Spacing();

    		ImGui::Text("Current Animation: ", animator.GetCurrentAnimationName().c_str());

    		std::string state_text = "STOPPED";
    		if (animator.GetState() == AnimationState::PLAYING)
    			state_text = "PLAYING";
    		else if (animator.GetState() == AnimationState::PAUSED)
    			state_text = "PAUSED";
    		ImGui::SameLine();
    		ImGui::Text("State: %s", state_text.c_str());
    		
    		ImGui::Checkbox("Loop", &animator.GetLoop());
    		ImGui::PushItemWidth(100.0f);
    		ImGui::SliderFloat("Speed", &animator.GetSpeed(), 0.0f, 10.0f);
    		ImGui::PopItemWidth();
    		ImGui::PushItemWidth(300.0f);
    		float progress = animator.GetCurrentAnimation() && animator.GetCurrentAnimation()->GetDuration() > 0.0f ? animator.GetCurrentAnimationTime() / animator.GetCurrentAnimation()->GetDuration() : 0.0f;
    		ImGui::ProgressBar(progress);
    		ImGui::PopItemWidth();

    		if (animator.GetState() == AnimationState::PLAYING)
    		{
    			if (ImGui::Button("Pause"))
    				animator.Pause();
    		}
    		else
    		{
    			if (ImGui::Button("Play"))
    			{
    				if (animator.GetCurrentAnimationName().empty())
    				{
    					if (!animator.GetAnimations().empty())
    					{
    						const auto& first_animation_name = animator.GetAnimations().begin()->first;
    						animator.Play(first_animation_name);
    					}
    				}
    				else
    					animator.Play(animator.GetCurrentAnimationName());
    			}
    		}
    		ImGui::SameLine();
    		if (ImGui::Button("Stop"))
    			animator.Stop();

    		ImGui::Spacing();

    		ImGui::Text("Animations");
    		ImGui::PushItemWidth(100.0f);
    		for (auto& [animation_name, animation] : animator.GetAnimations())
    		{
    			if (ImGui::Button(animation_name.c_str()))
    			{
    				animator.Stop();
    				animator.Play(animation_name);
    			}
    		}
    		ImGui::PopItemWidth();
    		
    		ImGui::PopID();
    	}
    }
	
    void Editor::DisplayAudioComponent()
    {
    	Scene& scene = Project::GetRenderer().GetScene();
    	
    	if (selected_entity->HasComponent<AudioComponent>())
    	{
    		AudioComponent& audio_component = selected_entity->GetComponent<AudioComponent>();
    		Audio& audio = *audio_component.audio;
    			
    		ImGui::PushID(&audio_component);
    		ImGui::Separator();
				
    		ImGui::Checkbox("##Enabled", &audio_component.enabled);
    		ImGui::SameLine();
    		ImGui::Text("Audio Component");
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(audio_component.id).c_str());
    		ImGui::Spacing();

    		bool loop = audio.GetLoop();
    		bool play_on_awake = audio.GetPlayOnAwake();
    		float volume = audio.GetVolume();
    		float pitch = audio.GetPitch();
    		if (ImGui::Checkbox("Loop", &loop))
    			audio.SetLoop(loop);
    		if (ImGui::Checkbox("Play On Awake", &play_on_awake))
    			audio.SetPlayOnAwake(play_on_awake);
    		if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%.2f"))
    			audio.SetVolume(volume);
    		if (ImGui::SliderFloat("Pitch", &pitch, 0.0f, 1.0f, "%.2f"))
    			audio.SetPitch(pitch);

    		if (ImGui::Button(audio_component.audio->name.c_str(), ImVec2(100, 22)))
    			ImGui::OpenPopup("ChangeAudioInAudioComponent");
    		ImGui::SameLine(); ImGui::Text("Audio");

    		AudioSystem& audio_system = Project::GetAudioSystem();
    		if (ImGui::BeginPopup("ChangeAudioInAudioComponent"))
    		{
    			for (auto& [id, scene_item] : audio_system.GetAudios())
    			{
    				ImGui::PushID(&id);
    				if (ImGui::Selectable(scene_item->name.c_str(), false, 0))
    				{
    					audio_component.audio->Stop();
    					audio_component.audio = scene_item;
    					ImGui::CloseCurrentPopup();
    				}
    				ImGui::PopID();
    			}
    			ImGui::EndPopup();
    		}
    			
    		ImGui::PopID();
    	}
    }
	
    void Editor::DisplayCameraComponent()
    {
    	Scene& scene = Project::GetRenderer().GetScene();
        
    	if (selected_entity->HasComponent<CameraComponent>())
    	{
    		CameraComponent& camera_component = selected_entity->GetComponent<CameraComponent>();

    		ImGui::PushID(&camera_component);
    		ImGui::Separator();

    		ImGui::Checkbox("##Enabled", &camera_component.enabled);
    		ImGui::SameLine();
    		ImGui::Text("Camera Component");
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(camera_component.id).c_str());
    		ImGui::Spacing();

    		ImGui::Text(std::to_string(camera_component.camera->id).c_str());
    		ImGui::DragFloat3("Position", (float*)&camera_component.camera->position, drag_step, -1000.0f, 1000.0f, "%.3f");
    		ImGui::SliderFloat("FOV", &camera_component.camera->fov, 1.0f, 120.0f, "%.f");
    		ImGui::PopID();
    	}
    }
	
    void Editor::DisplayScriptComponent()
    {
    	Scene& scene = Project::GetRenderer().GetScene();
    	if (selected_entity->HasComponent<ScriptComponent>())
    	{
    		ScriptComponent& script_component = selected_entity->GetComponent<ScriptComponent>();

    		ImGui::PushID(&script_component);
    		ImGui::Separator();

    		ImGui::Checkbox("##Enabled", &script_component.enabled);
    		ImGui::SameLine();
    		ImGui::Text("Script Component");
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(script_component.id).c_str());
    		ImGui::Spacing();

    		if (script_component.script)
    		{
    			if (ImGui::Button(script_component.script->name.c_str(), ImVec2(150, 22)))
    				ImGui::OpenPopup("ChangeScriptScriptComponent");
    			ImGui::SameLine();
    			ImGui::Text("Script");

    			if (ImGui::BeginPopup("ChangeScriptScriptComponent"))
    			{
    				ScriptSystem& script_system = Project::GetScriptSystem();
    				for (auto& [id, script] : script_system.GetScripts())
    				{
    					ImGui::PushID(&id);
    					if (ImGui::Selectable(script->name.c_str(), false, 0))
    					{
    						script_component.script = script;
    						ImGui::CloseCurrentPopup();
    					}
    					ImGui::PopID();
    				}
    				ImGui::EndPopup();
    			}
    		}

    		ImGui::PopID();
    	}
    }
}