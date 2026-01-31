#include "Editor.hpp"

void Editor::RemoveItems()
{
	if (entity_to_create != nullptr)
	{
		CreateEntity(entity_to_create);
		entity_to_create = nullptr;
	}
	if (entity_to_remove != nullptr)
	{
		RemoveEntity(entity_to_remove);
		entity_to_remove = nullptr;
	}
	if (entity_to_reparent != nullptr)
	{
		ReparentEntity(entity_to_reparent, reparent_target);
		entity_to_reparent = nullptr;
		reparent_target = nullptr;
	}
	if (selected_entity_to_remove_components != nullptr)
	{
		switch (component_to_remove)
		{
		case ComponentType::UNKNOWN: break;
		case ComponentType::CAMERA: RemoveCameraComponent(selected_entity_to_remove_components); break;
		case ComponentType::MODEL: RemoveModelComponent(selected_entity_to_remove_components); break;
		case ComponentType::LIGHT: RemoveLightSourceComponent(selected_entity_to_remove_components); break; 
		case ComponentType::PHYSICS: RemovePhysicsComponent(selected_entity_to_remove_components); break;
		case ComponentType::ANIMATION: RemoveAnimationComponent(selected_entity_to_remove_components); break;
		case ComponentType::AUDIO: RemoveAudioComponent(selected_entity_to_remove_components); break;
		case ComponentType::SCRIPT: RemoveScriptComponent(selected_entity_to_remove_components); break;
		}
		component_to_remove = ComponentType::UNKNOWN;
		selected_entity_to_remove_components = nullptr;
	}
}

void Editor::RemoveEntity(std::shared_ptr<Entity> entity)
{
	Engine& project = Engine::GetInstance();
	Window& project_window = project.GetWindow();
	Renderer& renderer = project.GetRenderer();
	Scene& scene = renderer.GetScene();
	
	std::function<void(std::shared_ptr<Entity>)> DeleteRecursive;
	DeleteRecursive = [&](std::shared_ptr<Entity> other_entity)
	{
		std::vector<uint32_t> children_copy = other_entity->children;
		for (uint32_t child_id : children_copy)
		{
			if (scene.GetEntities().contains(child_id))
			{
				DeleteRecursive(scene.GetEntities()[child_id]);
			}
		}

		if (other_entity->HasComponent<CameraComponent>()) RemoveCameraComponent(other_entity);
		if (other_entity->HasComponent<ModelComponent>()) RemoveModelComponent(other_entity);
		if (other_entity->HasComponent<LightSourceComponent>()) RemoveLightSourceComponent(other_entity);
		if (other_entity->HasComponent<PhysicsComponent>()) RemovePhysicsComponent(other_entity);
		if (other_entity->HasComponent<AnimationComponent>()) RemoveAnimationComponent(other_entity);
		if (other_entity->HasComponent<AudioComponent>()) RemoveAudioComponent(other_entity);
		if (other_entity->HasComponent<ScriptComponent>()) RemoveScriptComponent(other_entity);

		if (selected_entity == other_entity)
			selected_entity = nullptr;

		scene.GetEntities().erase(other_entity->id);
	};

	if (!entity->IsRoot() && scene.GetEntities().contains(entity->parent))
	{
		scene.GetEntities()[entity->parent]->RemoveChild(entity->id);
	}

	DeleteRecursive(entity);

	if (selected_entity == nullptr && !scene.GetEntities().empty())
		selected_entity = scene.GetEntities().begin()->second;
}

void Editor::RemoveCameraComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	auto& camera_component = entity->GetComponent<CameraComponent>();
	scene.GetCameras().erase(camera_component.camera->id);
	scene.GetCameraComponents().erase(camera_component.id);
	entity->RemoveComponent(ComponentType::CAMERA);

	if (scene.GetCameras().empty())
	{
		std::shared_ptr<Camera> default_camera = std::make_shared<Camera>(0);
		scene.GetCameras().insert_or_assign(0, default_camera);
		scene.SetCurrentCamera(0);
		return;
	}
	scene.SetCurrentCamera(scene.GetCameras().begin()->first);
}

void Editor::RemoveModelComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	auto& model_component = entity->GetComponent<ModelComponent>();
	scene.GetModelComponents().erase(model_component.id);
	entity->RemoveComponent(ComponentType::MODEL);
}

void Editor::RemoveLightSourceComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	auto& light_source_component = entity->GetComponent<LightSourceComponent>();
	if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
		scene.GetPointLights().erase(point_light->id);
	if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_source_component.light_source))
		scene.GetSpotLights().erase(spot_light->id);
	scene.GetLightSourceComponents().erase(light_source_component.id);
	entity->RemoveComponent(ComponentType::LIGHT);
}

void Editor::RemovePhysicsComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	PhysicsManager& physics_system = Engine::GetPhysicsSystem();
	JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
	auto& physics_component = entity->GetComponent<PhysicsComponent>();
	JPH::BodyID body_id = physics_component.physics_body->GetBodyID();
	if (body_interface.IsAdded(body_id))
	{
		body_interface.RemoveBody(body_id);
		body_interface.DestroyBody(body_id);
	}
	scene.GetPhysicsComponents().erase(physics_component.id);
	entity->RemoveComponent(ComponentType::PHYSICS);
}

void Editor::RemoveAnimationComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	auto& animation_component = entity->GetComponent<AnimationComponent>();
	animation_component.animator->Stop();
	scene.GetAnimationComponents().erase(animation_component.id);
	entity->RemoveComponent(ComponentType::ANIMATION);
}

void Editor::RemoveAudioComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	entity->GetComponent<AudioComponent>().audio->Stop();
	scene.GetAudioComponents().erase(entity->GetComponent<AudioComponent>().id);
	entity->RemoveComponent(ComponentType::AUDIO);
}

void Editor::RemoveScriptComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	auto& script_component = entity->GetComponent<ScriptComponent>();
	scene.GetScriptComponents().erase(script_component.id);
	entity->RemoveComponent(ComponentType::SCRIPT);
}