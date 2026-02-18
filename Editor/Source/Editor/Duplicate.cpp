#include "Editor.hpp"

void Editor::DuplicateEntity(const std::shared_ptr<Entity>& root_entity)
{
	Renderer& renderer = Engine::GetRenderer();
	Scene& scene = renderer.GetScene();
	
    std::function<uint32_t(std::shared_ptr<Entity>)> DuplicateRecursive;
	DuplicateRecursive = [&](std::shared_ptr<Entity> original_entity) -> uint32_t
	{
		uint32_t next_entity_id = FIRST_ID;
		if (!scene.GetEntities().empty())
		{
			auto max_it = std::ranges::max_element(scene.GetEntities(), 
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_entity_id = max_it->first + 1;
		}

		std::shared_ptr<Entity> duplicate_entity = std::make_shared<Entity>(*original_entity);
		duplicate_entity->id = next_entity_id;
		std::vector<uint32_t> original_children = duplicate_entity->children;
		duplicate_entity->children.clear();

		if (original_entity->HasComponent<ModelComponent>())
			DuplicateModelComponent(original_entity, duplicate_entity, scene);
		if (original_entity->HasComponent<LightSourceComponent>())
			DuplicateLightSourceComponent(original_entity, duplicate_entity, scene);
		if (original_entity->HasComponent<PhysicsComponent>())
			DuplicatePhysicsComponent(original_entity, duplicate_entity, scene);
		if (original_entity->HasComponent<AnimationComponent>())
			DuplicateAnimationComponent(original_entity, duplicate_entity, scene);
		if (original_entity->HasComponent<AudioComponent>())
			DuplicateAudioComponent(original_entity, duplicate_entity, scene);
		if (original_entity->HasComponent<ScriptComponent>())
			DuplicateScriptComponent(original_entity, duplicate_entity, scene);
		if (original_entity->HasComponent<CameraComponent>())
			DuplicateCameraComponent(original_entity, duplicate_entity, scene);
		
	    scene.GetEntities().insert_or_assign(next_entity_id, duplicate_entity);

	    for (uint32_t child_id : original_children)
	    {
		    if (scene.GetEntities().contains(child_id))
		    {
			    uint32_t new_child_id = DuplicateRecursive(scene.GetEntities().at(child_id));
			    duplicate_entity->children.push_back(new_child_id);
			    scene.GetEntities().at(new_child_id)->parent = next_entity_id;
		    }
	    }

	    return next_entity_id;
	};

	uint32_t original_parent = root_entity->parent;
	bool original_is_root = root_entity->IsRoot();
	uint32_t new_root_id = DuplicateRecursive(root_entity);

	scene.GetEntities().at(new_root_id)->parent = original_parent;

	if (!original_is_root && scene.GetEntities().contains(original_parent))
		scene.GetEntities().at(original_parent)->children.push_back(new_root_id);

	selected_entity = scene.GetEntities().at(new_root_id);
}

void Editor::DuplicateModelComponent(std::shared_ptr<Entity> original_entity, std::shared_ptr<Entity> duplicate_entity, Scene& scene)
{
	auto& original_component = original_entity->GetComponent<ModelComponent>();

	uint32_t next_comp_id = FIRST_ID;
	if (!scene.GetModelComponents().empty())
	{
		auto max_comp = std::ranges::max_element(scene.GetModelComponents(),
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

	scene.GetModelComponents().insert_or_assign(next_comp_id, new_component);
	duplicate_entity->RemoveComponent(ComponentType::MODEL);
	duplicate_entity->AddComponent(ComponentType::MODEL, new_component);
}

void Editor::DuplicateLightSourceComponent(std::shared_ptr<Entity> original_entity, std::shared_ptr<Entity> duplicate_entity, Scene& scene)
{
    auto& original_component = original_entity->GetComponent<LightSourceComponent>();

    // Generate new component ID
    uint32_t next_comp_id = FIRST_ID;
    if (!scene.GetLightSourceComponents().empty())
    {
            auto max_comp = std::ranges::max_element(scene.GetLightSourceComponents(),
            	[](const auto& a, const auto& b) { return a.first < b.first; }
            );
            next_comp_id = max_comp->first + 1;
    }

    uint32_t next_light_id = FIRST_ID;

    std::shared_ptr<LightSource> new_light_source;

    if (auto point_light = std::dynamic_pointer_cast<PointLight>(original_component.light_source))
    {
            if (!scene.GetPointLights().empty())
            {
                    auto max_light = std::ranges::max_element(scene.GetPointLights(),
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
            scene.GetPointLights().insert_or_assign(next_light_id, new_point_light);
    }
    else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(original_component.light_source))
    {
            if (!scene.GetSpotLights().empty())
            {
                    auto max_light = std::ranges::max_element(scene.GetSpotLights(),
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
            scene.GetSpotLights().insert_or_assign(next_light_id, new_spot_light);
    }

    std::shared_ptr<LightSourceComponent> new_component = std::make_shared<LightSourceComponent>(
            next_comp_id,
            original_component.enabled,
            new_light_source
    );

    scene.GetLightSourceComponents().insert_or_assign(next_comp_id, new_component);
    duplicate_entity->RemoveComponent(ComponentType::LIGHT);
    duplicate_entity->AddComponent(ComponentType::LIGHT, new_component);
}

void Editor::DuplicatePhysicsComponent(std::shared_ptr<Entity> original_entity, std::shared_ptr<Entity> duplicate_entity, Scene& scene)
{
    PhysicsManager& physics_system = Engine::GetPhysicsManager();
    auto& original_component = original_entity->GetComponent<PhysicsComponent>();

    uint32_t next_comp_id = FIRST_ID;
    if (!scene.GetPhysicsComponents().empty())
    {
        auto max_comp = std::ranges::max_element(scene.GetPhysicsComponents(),
        	[](const auto& a, const auto& b) { return a.first < b.first; }
        );
        next_comp_id = max_comp->first + 1;
    }

    uint32_t next_physics_id = FIRST_ID;
    if (!scene.GetPhysicsComponents().empty())
    {
        uint32_t max_physics_id = FIRST_ID;
        for (const auto& [id, comp] : scene.GetPhysicsComponents())
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
    else if (shape_data.type == PhysicsShapeType::MESH)
    {
        uint32_t model_id = original_component.physics_body->mesh_id;

        if (original_entity->HasComponent<ModelComponent>())
        {
	        ModelComponent& model_component = original_entity->GetComponent<ModelComponent>();

	        new_physics_body = physics_system.CreateMeshBody(
				original_entity->position,
				glm::quat(glm::radians(original_entity->rotation)),
				model_component.model,
				model_id,
				original_component.physics_body->GetBodyType()
			);
        }
    }

    if (new_physics_body)
    {
        new_physics_body->id = next_physics_id;
        new_physics_body->enabled = enabled;
        new_physics_body->SetEnabled(enabled);
        new_physics_body->name = name;

        std::shared_ptr<PhysicsComponent> new_component = std::make_shared<PhysicsComponent>(
            next_comp_id,
            original_component.enabled,
            new_physics_body,
            original_component.can_move_axis,
			original_component.can_rotate_axis
            );

        new_physics_body->SetAllowedDOFS(
	        original_component.can_move_axis[0], original_component.can_move_axis[1], original_component.can_move_axis[2],
	        original_component.can_rotate_axis[0], original_component.can_rotate_axis[1], original_component.can_rotate_axis[2]);

        scene.GetPhysicsComponents().insert_or_assign(next_comp_id, new_component);
        duplicate_entity->RemoveComponent(ComponentType::PHYSICS);
        duplicate_entity->AddComponent(ComponentType::PHYSICS, new_component);
    }
}

void Editor::DuplicateAnimationComponent(std::shared_ptr<Entity> original_entity, std::shared_ptr<Entity> duplicate_entity, Scene& scene)
{
	AnimationComponent& original_component = original_entity->GetComponent<AnimationComponent>();

	uint32_t next_comp_id = FIRST_ID;
	if (!scene.GetAnimationComponents().empty())
	{
		auto max_comp = std::ranges::max_element(scene.GetAnimationComponents(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_comp_id = max_comp->first + 1;
	}

	std::shared_ptr<Animator> new_animator = nullptr;
	if (original_component.animator)
		new_animator = std::make_shared<Animator>(*original_component.animator);
	std::shared_ptr<AnimationComponent> new_component = std::make_shared<AnimationComponent>(next_comp_id, original_component.enabled, new_animator);

	scene.GetAnimationComponents().insert_or_assign(next_comp_id, new_component);
	duplicate_entity->RemoveComponent(ComponentType::ANIMATION);
	duplicate_entity->AddComponent(ComponentType::ANIMATION, new_component);
}

void Editor::DuplicateAudioComponent(std::shared_ptr<Entity> original_entity, std::shared_ptr<Entity> duplicate_entity, Scene& scene)
{
	AudioComponent& original_component = original_entity->GetComponent<AudioComponent>();
	uint32_t next_comp_id = FIRST_ID;
	if (!scene.GetAudioComponents().empty())
	{
		auto max_comp = std::ranges::max_element(scene.GetAudioComponents(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_comp_id = max_comp->first + 1;
	}

	std::shared_ptr<Audio> new_audio = nullptr;
	if (original_component.audio)
	{
		AudioSystem& audio_system = Engine::GetAudioManager();
		uint32_t next_audio_id = FIRST_ID;
		if (!audio_system.GetAudios().empty())
		{
			auto max_comp = std::ranges::max_element(audio_system.GetAudios(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_audio_id = max_comp->first + 1;
		}
		new_audio = std::make_shared<Audio>(*original_component.audio);
		if (new_audio)
		{
			new_audio->SetVolume(original_component.audio->GetVolume());
			new_audio->SetPitch(original_component.audio->GetPitch());
			new_audio->SetLoop(original_component.audio->GetLoop());
			new_audio->SetPlayOnAwake(original_component.audio->GetPlayOnAwake());
			new_audio->id = next_audio_id;
		}
	}

	std::shared_ptr<AudioComponent> new_component = std::make_shared<AudioComponent>(next_comp_id, original_component.enabled, new_audio);
	scene.GetAudioComponents().insert_or_assign(next_comp_id, new_component);
	duplicate_entity->RemoveComponent(ComponentType::AUDIO);
	duplicate_entity->AddComponent(ComponentType::AUDIO, new_component);
}

void Editor::DuplicateScriptComponent(std::shared_ptr<Entity> original_entity, std::shared_ptr<Entity> duplicate_entity, Scene& scene)
{
	
	ScriptComponent& original_component = original_entity->GetComponent<ScriptComponent>();
	uint32_t next_comp_id = FIRST_ID;
	if (!scene.GetScriptComponents().empty())
	{
		auto max_comp = std::ranges::max_element(scene.GetScriptComponents(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_comp_id = max_comp->first + 1;
	}

	std::shared_ptr<ScriptComponent> new_component = std::make_shared<ScriptComponent>(
		next_comp_id,
		original_component.enabled,
		original_component.script
	);
	scene.GetScriptComponents().insert_or_assign(next_comp_id, new_component);
	duplicate_entity->RemoveComponent(ComponentType::SCRIPT);
	duplicate_entity->AddComponent(ComponentType::SCRIPT, new_component);
}

void Editor::DuplicateCameraComponent(std::shared_ptr<Entity> original_entity, std::shared_ptr<Entity> duplicate_entity, Scene& scene)
{
	uint32_t next_comp_id = FIRST_ID;
	if (!scene.GetCameraComponents().empty())
	{
		auto max_comp = std::ranges::max_element(scene.GetCameraComponents(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_comp_id = max_comp->first + 1;
	}
	uint32_t next_camera_id = FIRST_ID;
	if (!scene.GetCameras().empty())
	{
		auto max_comp = std::ranges::max_element(scene.GetCameras(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_camera_id = max_comp->first + 1;
	}
			
	std::shared_ptr<Camera> new_camera = std::make_shared<Camera>(next_camera_id, duplicate_entity->position);
	scene.GetCameras().insert_or_assign(next_camera_id, new_camera);
	std::shared_ptr<CameraComponent> new_camera_component = std::make_shared<CameraComponent>(next_comp_id, true, new_camera);
	scene.GetCameraComponents().insert_or_assign(next_comp_id, new_camera_component);
	duplicate_entity->RemoveComponent(ComponentType::CAMERA);
	duplicate_entity->AddComponent(ComponentType::CAMERA, new_camera_component);
}