#include "bonfire_pch.hpp"
#include "Editor.hpp"

#include "Commands.hpp"
#include "Core/Utility.hpp"
#include "Core/Project.hpp"

namespace Bonfire
{
	void Editor::CreateEntity(std::shared_ptr<Entity> parent)
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		uint32_t next_id = 1000001;
		if (!scene.GetEntities().empty())
		{
			auto max_it = std::max_element(
			  scene.GetEntities().begin(),
			  scene.GetEntities().end(),
			  [](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}
		std::shared_ptr<Entity> new_entity = std::make_shared<Entity>(next_id);
		if (parent != nullptr)
		{
			new_entity->parent = parent->id;
			scene.GetEntities().at(parent->id)->AddChild(new_entity->id);
		}
		scene.GetEntities().insert_or_assign(next_id, new_entity);
	}
	
    void Editor::CreateModelComponent(std::shared_ptr<Entity> entity)
	{
		Scene& scene = Project::GetRenderer().GetScene();
		
		uint32_t next_id = 100001;
		if (!scene.GetModelComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetModelComponents().begin(),
				scene.GetModelComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
				);
			next_id = max_it->first + 1;
		}

		if (!scene.GetModels().empty())
		{
			std::shared_ptr<Model> default_model = scene.GetModels().begin()->second;
			std::shared_ptr<Shader> default_shader = scene.GetShaders().begin()->second;
			std::shared_ptr<Material> default_material = scene.GetMaterials().begin()->second;
			std::shared_ptr<ModelComponent> new_component = std::make_shared<ModelComponent>(next_id, true, default_model, default_shader, default_material);

			scene.GetModelComponents().insert_or_assign(next_id, new_component);
			entity->AddComponent(ComponentType::MODEL, new_component);
		}
		else
			Log::Warning("No models available");
	}
	void Editor::CreateLightSourceComponent(std::shared_ptr<Entity> entity)
	{
		Scene& scene = Project::GetRenderer().GetScene();
		
		uint32_t next_id = 100001;
		if (!scene.GetLightSourceComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetLightSourceComponents().begin(),
				scene.GetLightSourceComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}

		uint32_t next_light_id = 1000;
		if (!scene.GetPointLights().empty())
		{
			auto max_it = std::max_element(
				scene.GetPointLights().begin(),
				scene.GetPointLights().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_light_id = max_it->first + 1;
		}
    					
		std::shared_ptr<PointLight> new_light = std::make_shared<PointLight>();
		new_light->id = next_light_id;
		new_light->position = entity->position;
		new_light->color = glm::vec3(255.0f, 255.0f, 255.0f);
		new_light->scale = glm::vec3(1.0f);
		new_light->intensity = 1.0f;
		new_light->enabled = true;

		std::shared_ptr<LightSourceComponent> new_component = std::make_shared<LightSourceComponent>(next_id, true, new_light);

		scene.GetLightSourceComponents().insert_or_assign(next_id, new_component);
		scene.GetPointLights().insert_or_assign(next_light_id, new_light);
		entity->AddComponent(ComponentType::LIGHT, new_component);
	}
	void Editor::CreatePhysicsComponent(std::shared_ptr<Entity> entity)
	{
		Scene& scene = Project::GetRenderer().GetScene();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		
		uint32_t next_id = 100001;
		if (!scene.GetPhysicsComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetPhysicsComponents().begin(),
				scene.GetPhysicsComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}
    					
		uint32_t next_po_id = 1000;
		if (!scene.GetPhysicsComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetPhysicsComponents().begin(),
				scene.GetPhysicsComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_po_id = max_it->first + 1;
		}

		std::string physics_name = "Physics Object";
		PhysicsBodyType body_type = PhysicsBodyType::STATIC;
		PhysicsShapeType shape_type = PhysicsShapeType::CAPSULE;
		glm::vec3 dimensions = glm::vec3(1.0f, 1.0f, 0.0f);

		std::shared_ptr<PhysicsBody> physics_body = physics_system.CreateCapsuleBody(entity->position, glm::quat(glm::radians(entity->rotation)), dimensions.x, dimensions.y, body_type);
		physics_body->id = next_po_id;
		physics_body->enabled = true;
		physics_body->SetEnabled(true);
		physics_body->name = physics_name;
		physics_body->SetPosition(entity->position);
		physics_body->SetRotation(glm::quat(glm::radians(entity->rotation)));
		physics_body->SetScale(entity->scale);
		
		std::array<bool, 3> default_can_move_axis = { true, true, true };
		std::array<bool, 3> default_can_rotate_axis = { true, true, true };
		
		physics_body->SetAllowedDOFS(
			default_can_move_axis[0], default_can_move_axis[1], default_can_move_axis[2],
			default_can_rotate_axis[0], default_can_rotate_axis[1], default_can_rotate_axis[2]
		);

		std::shared_ptr<PhysicsComponent> physics_component = std::make_shared<PhysicsComponent>(next_id, true, physics_body, default_can_move_axis, default_can_rotate_axis);
		scene.GetPhysicsComponents().insert_or_assign(next_id, physics_component);
		entity->AddComponent(ComponentType::PHYSICS, physics_component);
	}
	void Editor::CreateAnimationComponent(std::shared_ptr<Entity> entity)
	{
		Scene& scene = Project::GetRenderer().GetScene();
		
		if (!entity->HasComponent<ModelComponent>())
    	{
    		Log::Warning("Entity must have model to add animator");
			ImGui::CloseCurrentPopup();
			return;
    	}
		
		ModelComponent& model_component = entity->GetComponent<ModelComponent>();
		if (!model_component.model->IsAnimated())
		{
			Log::Warning("Model must be animated (skeletal) to add animator");
			ImGui::CloseCurrentPopup();
			return;
		}
		
	    std::shared_ptr<SkeletalModel> skeletal_model = std::static_pointer_cast<SkeletalModel>(model_component.model);

	    if (skeletal_model->GetAnimations().empty())
	    {
	    	Log::Warning("Skeletal model has no animations loaded");
	    	ImGui::CloseCurrentPopup();
	    }
	    else
	    {
	    	uint32_t next_id = 100001;
	    	if (!scene.GetAnimationComponents().empty())
	    	{
	    		auto max_it = std::max_element(
					scene.GetAnimationComponents().begin(),
					scene.GetAnimationComponents().end(),
					[](const auto& a, const auto& b) { return a.first < b.first; }
				);
	    		next_id = max_it->first + 1;
	    	}

	    	std::shared_ptr<Animator> animator = std::make_shared<Animator>(skeletal_model->GetSkeleton());

	    	for (const auto& animation : skeletal_model->GetAnimations())
	    	{
	    		animator->AddAnimation(animation);
	    	}

	    	std::shared_ptr<AnimationComponent> anim_comp = std::make_shared<AnimationComponent>(next_id, true, animator);

	    	scene.GetAnimationComponents().insert_or_assign(next_id, anim_comp);
	    	entity->AddComponent(ComponentType::ANIMATION, anim_comp);

	    	Log::Info("Added Animation Component with " + std::to_string(skeletal_model->GetAnimations().size()) + " animation(s)");

	    	for (const auto& anim : skeletal_model->GetAnimations())
	    	{
	    		Log::Info("  - " + anim->GetName());
	    	}
	    }
	}
	void Editor::CreateAudioComponent(std::shared_ptr<Entity> entity)
	{
		Scene& scene = Project::GetRenderer().GetScene();
		AudioSystem& audio_system = Project::GetAudioSystem();
		
		uint32_t next_id = 100001;
		if (!scene.GetAudioComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetAudioComponents().begin(),
				scene.GetAudioComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}

		if (audio_system.GetAudios().empty())
		{
			Log::Warning("No audios found in Audio Params");
			return;
		}
		std::shared_ptr<Audio> audio = audio_system.GetAudios().begin()->second;
		std::shared_ptr<AudioComponent> audio_component = std::make_shared<AudioComponent>(next_id, true, audio);
		scene.GetAudioComponents().insert_or_assign(next_id, audio_component);
		entity->AddComponent(ComponentType::AUDIO, audio_component);
	}
	void Editor::CreateScriptComponent(std::shared_ptr<Entity> entity)
	{
		Scene& scene = Project::GetRenderer().GetScene();
		ScriptSystem& script_system = Project::GetScriptSystem();

		uint32_t next_id = 100001;
		if (!scene.GetScriptComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetScriptComponents().begin(),
				scene.GetScriptComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}

		if (script_system.GetScripts().empty())
		{
			Log::Warning("No scripts found in Script Params");
			return;
		}

		std::shared_ptr<LuaScript> script = script_system.GetScripts().begin()->second;
		std::shared_ptr<ScriptComponent> script_component = std::make_shared<ScriptComponent>(next_id, true, script);
		scene.GetScriptComponents().insert_or_assign(next_id, script_component);
		entity->AddComponent(ComponentType::SCRIPT, script_component);
	}
	void Editor::CreateCameraComponent(std::shared_ptr<Entity> entity)
	{
		Scene& scene = Project::GetRenderer().GetScene();

		uint32_t next_id = 100001;
		if (!scene.GetCameraComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetCameraComponents().begin(),
				scene.GetCameraComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}

		bool is_first_camera = false;
		if (scene.GetCameras().begin()->second->id == 0)
		{
			scene.GetCameras().erase(0);
			is_first_camera = true;
		}
		
		uint32_t next_camera_id = 1000;
		if (!scene.GetCameras().empty())
		{
			auto max_it = std::max_element(
				scene.GetCameras().begin(),
				scene.GetCameras().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_camera_id = max_it->first + 1;
		}

		std::shared_ptr<Camera> new_camera = std::make_shared<Camera>(next_camera_id, entity->position);
		scene.GetCameras().insert_or_assign(next_camera_id, new_camera);
		std::shared_ptr<CameraComponent> new_camera_component = std::make_shared<CameraComponent>(next_id, true, new_camera);
		scene.GetCameraComponents().insert_or_assign(next_id, new_camera_component);
		entity->AddComponent(ComponentType::CAMERA, new_camera_component);
		if (is_first_camera)
			scene.SetCurrentCamera(next_camera_id);
	}
}