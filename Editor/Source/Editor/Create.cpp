#include "Editor.hpp"

bool Editor::CreateEntity(std::shared_ptr<Entity> parent)
{
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
	Scene& scene = renderer.GetScene();

	uint32_t next_id = FIRST_ID;
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

	return true;
}

bool Editor::CreateModelComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();

	uint32_t next_id = FIRST_ID;
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
		return true;
	}
	Log::Warning("No models available");
	return false;
}

bool Editor::CreateLightSourceComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();

	uint32_t next_id = FIRST_ID;
	if (!scene.GetLightSourceComponents().empty())
	{
		auto max_it = std::max_element(
			scene.GetLightSourceComponents().begin(),
			scene.GetLightSourceComponents().end(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_id = max_it->first + 1;
	}

	uint32_t next_light_id = FIRST_ID;
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

	return true;
}

bool Editor::CreatePhysicsComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	PhysicsManager& physics_system = Engine::GetPhysicsManager();

	uint32_t next_id = FIRST_ID;
	if (!scene.GetPhysicsComponents().empty())
	{
		auto max_it = std::max_element(
			scene.GetPhysicsComponents().begin(),
			scene.GetPhysicsComponents().end(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_id = max_it->first + 1;
	}

	uint32_t next_po_id = FIRST_ID;
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

	return true;
}

bool Editor::CreateAnimationComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();

	if (!entity->HasComponent<ModelComponent>())
    {
    	Log::Warning("Entity must have model to add animator");
		ImGui::CloseCurrentPopup();
		return false;
    }

	ModelComponent& model_component = entity->GetComponent<ModelComponent>();
	if (!model_component.model->IsAnimated())
	{
		Log::Warning("Model must be animated (skeletal) to add animator");
		ImGui::CloseCurrentPopup();
		return false;
	}

    std::shared_ptr<SkeletalModel> skeletal_model = std::static_pointer_cast<SkeletalModel>(model_component.model);

    if (skeletal_model->GetAnimations().empty())
    {
	    Log::Warning("Skeletal model has no animations loaded");
	    ImGui::CloseCurrentPopup();
		return false;
    }

    uint32_t next_id = FIRST_ID;
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

	return true;
}

bool Editor::CreateAudioComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();
	AudioSystem& audio_system = Engine::GetAudioManager();

	uint32_t next_id = FIRST_ID;
	if (!scene.GetAudioComponents().empty())
	{
		auto max_it = std::max_element(
			scene.GetAudioComponents().begin(),
			scene.GetAudioComponents().end(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_id = max_it->first + 1;
	}

	uint32_t next_audio_id = FIRST_ID;
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
		Log::Warning("Failed to find default audio in Audio Params");
		return false;
	}

	std::string new_audio_path = audio_system.GetAudios().begin()->second->path;
	std::shared_ptr<Audio> audio = std::make_shared<Audio>(next_audio_id, "New Audio", new_audio_path);
	audio->Set3DPosition(entity->position);
	Engine::GetAudioManager().AddAudio(audio);
	std::shared_ptr<AudioComponent> audio_component = std::make_shared<AudioComponent>(next_id, true, audio);
	scene.GetAudioComponents().insert_or_assign(next_id, audio_component);
	entity->AddComponent(ComponentType::AUDIO, audio_component);

	return true;
}

bool Editor::CreateScriptComponent(std::shared_ptr<Entity> entity)
{
	return false;

	/*Scene& scene = Engine::GetRenderer().GetScene();
	ScriptManager& script_system = Engine::GetScriptSystem();

	uint32_t next_id = FIRST_ID;
	if (!scene.GetScriptComponents().empty())
	{
		auto max_it = std::max_element(
			scene.GetScriptComponents().begin(),
			scene.GetScriptComponents().end(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
		);
		next_id = max_it->first + 1;
	}

	if (script_system.GetLuaScripts().empty())
	{
		Log::Warning("No scripts found in Script Params");
		return false;
	}

	std::shared_ptr<LuaScript> script = script_system.GetLuaScripts().begin()->second;
	std::shared_ptr<ScriptComponent> script_component = std::make_shared<ScriptComponent>(next_id, true, script);
	scene.GetScriptComponents().insert_or_assign(next_id, script_component);
	entity->AddComponent(ComponentType::SCRIPT, script_component);

	return true;*/
}

bool Editor::CreateCameraComponent(std::shared_ptr<Entity> entity)
{
	Scene& scene = Engine::GetRenderer().GetScene();

	uint32_t next_id = FIRST_ID;
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

	uint32_t next_camera_id = FIRST_ID;
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

	return true;
}

bool Editor::CreateMaterialParam()
{
	Renderer& renderer = Engine::GetRenderer();
	Scene& scene = renderer.GetScene();
	ParamDatabase& param_database = renderer.GetParamDatabase();

	uint32_t next_id = FIRST_ID;
	if (!scene.GetMaterials().empty())
	{
		auto max_it = std::max_element(
			scene.GetMaterials().begin(),
			scene.GetMaterials().end(),
			[](const auto& a, const auto& b) { return a.first < b.first; }
			);
		next_id = max_it->first + 1;
	}

	std::shared_ptr<Material> new_material = std::make_shared<Material>("New Material");
	new_material->param_id = next_id;
	new_material->AddTexture(default_textures.at(0));
	new_material->AddTexture(default_textures.at(1));
	new_material->AddTexture(default_textures.at(2));
	new_material->AddTexture(default_textures.at(3));
	new_material->AddTexture(default_textures.at(4));
	new_material->shininess = 64.0f;
	new_material->texture_tiling = glm::vec2(1.0f, 1.0f);
	new_material->texture_offset = glm::vec2(0.0f, 0.0f);

	scene.GetMaterials().insert_or_assign(next_id, new_material);
	param_database.material_params.insert_or_assign(
		next_id,
		MaterialParamData(new_material->name,
		new_material->GetTexture(TextureType::DIFFUSE)->param_id, new_material->GetTexture(TextureType::SPECULAR)->param_id, new_material->GetTexture(TextureType::NORMAL)->param_id,
		new_material->GetTexture(TextureType::HEIGHT)->param_id, new_material->GetTexture(TextureType::EMISSION)->param_id,
		new_material->shininess, new_material->texture_tiling, new_material->texture_offset)
		);
	selected_material_param_id = next_id;

	return true;
}

bool Editor::CreateModelParam()
{
	Renderer& renderer = Engine::GetRenderer();
	Scene& scene = renderer.GetScene();
	ParamDatabase& param_database = renderer.GetParamDatabase();

    std::string model_file;

#ifdef BONFIRE_PLATFORM_WINDOWS
    {
        char exe_path[MAX_PATH];
        GetModuleFileNameA(NULL, exe_path, MAX_PATH);
        std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
        std::filesystem::path models_dir = exe_dir / "Data/Resources/Models";
        std::string file_buf = std::string(MAX_PATH, '\0');
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFile = (LPSTR)file_buf.c_str();
        ofn.nMaxFile = file_buf.size();
        ofn.lpstrInitialDir = models_dir.string().c_str();
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrFilter = "Model Files\0*.obj;*.fbx;*.dae;*.gltf;*.glb\0Obj Files\0*.obj\0FBX Files\0*.fbx\0DAE Files\0*.dae\0glTF Files\0*.gltf;*.glb\0All Files\0*.*\0";
        ofn.lpstrTitle = "Select model file";
        if (!GetOpenFileNameA(&ofn))
        {
            Log::Info("File operation cancelled");
            ImGui::CloseCurrentPopup();
            return false;
        }
        file_buf.resize(file_buf.find('\0'));
        model_file = file_buf;
    }
#elif defined(BONFIRE_PLATFORM_LINUX)
    {
        FILE* f = popen("zenity --file-selection --title=\"Select model file\" --file-filter=\"Model Files | *.obj *.fbx *.dae *.gltf *.glb\" 2>/dev/null", "r");
        if (!f) { Log::Info("File operation cancelled"); ImGui::CloseCurrentPopup(); return false; }
        char buf[4096] = {};
        bool ok = fgets(buf, sizeof(buf), f) != nullptr;
        pclose(f);
        if (!ok || buf[0] == '\0') { Log::Info("File operation cancelled"); ImGui::CloseCurrentPopup(); return false; }
        model_file = buf;
        if (!model_file.empty() && model_file.back() == '\n') model_file.pop_back();
    }
#endif

    std::filesystem::path absolute_path = model_file;
        std::string abs_str = absolute_path.string();

        size_t data_pos = abs_str.find("Data");
        if (data_pos != std::string::npos)
        {
            default_model_path = abs_str.substr(data_pos);
            std::replace(default_model_path.begin(), default_model_path.end(), '\\', '/');
        }
        else
            default_model_path = model_file;

        Log::Info("File selected at " + default_model_path);

        uint32_t next_id = FIRST_ID;
        if (!scene.GetModels().empty())
        {
            auto max_it = std::max_element(
                scene.GetModels().begin(),
                scene.GetModels().end(),
                [](const auto& a, const auto& b) { return a.first < b.first; }
                );
            next_id = max_it->first + 1;
        }

        std::filesystem::path path_obj(default_model_path);
        std::string model_name = path_obj.stem().string();

        bool has_animations = false;
        int animation_count = 0;

        std::filesystem::path absolute_model_path = std::filesystem::absolute(default_model_path);
        std::string abs_path_str = absolute_model_path.string();

        Assimp::Importer temp_importer;
        const aiScene* temp_scene = temp_importer.ReadFile(abs_path_str, aiProcess_ValidateDataStructure | 0);

        if (!temp_scene)
        {
            Log::Warning("Assimp pre-scan failed: " + std::string(temp_importer.GetErrorString()));
            Log::Info("Will attempt to load as regular model");
        }
        else if (temp_scene->HasAnimations() && temp_scene->mNumAnimations > 0)
        {
            has_animations = true;
            animation_count = temp_scene->mNumAnimations;
            Log::Info("Found " + std::to_string(animation_count) + " animation(s)");

            for (unsigned int i = 0; i < temp_scene->mNumAnimations; i++)
            {
                aiAnimation* anim = temp_scene->mAnimations[i];
                std::string anim_name = anim->mName.C_Str();
                if (anim_name.empty())
                    anim_name = "Animation_" + std::to_string(i);
                Log::Info("  - " + anim_name + " (" + std::to_string(anim->mDuration) + " ticks, " +
                          std::to_string(anim->mTicksPerSecond) + " tps)");
            }
        }
        else
            Log::Info("No animations found in file");

        std::shared_ptr<Model> new_model;
        if (has_animations)
        {
            Log::Info("Creating SkeletalModel");
            new_model = std::make_shared<SkeletalModel>(default_model_path);
            SkeletalModel* skel_model = static_cast<SkeletalModel*>(new_model.get());
            if (skel_model->GetAnimations().empty())
                Log::Warning("SkeletalModel created but no animations loaded!");
            else
                Log::Info("Successfully loaded " + std::to_string(skel_model->GetAnimations().size()) + " animations");
        }
        else
        {
            Log::Info("Creating regular Model");
            new_model = std::make_shared<Model>(default_model_path);
            new_model->Load();
        }

        new_model->param_id = next_id;
        new_model->name = model_name;

        scene.GetModels().insert_or_assign(next_id, new_model);
        param_database.model_params.insert_or_assign(next_id, ModelParamData(model_name, default_model_path, new_model->IsAnimated()));
        selected_model_param_id = next_id;

        for (auto& [id, entity] : scene.GetEntities())
        {
            if (entity->HasComponent<ModelComponent>())
            {
                ModelComponent& model_component = entity->GetComponent<ModelComponent>();
                if (model_component.model->param_id == selected_model_param_id)
                    model_component.model->param_id = FIRST_ID;
            }
        }
    return true;
}

bool Editor::CreateTextureParam()
{
	Renderer& renderer = Engine::GetRenderer();
	Scene& scene = renderer.GetScene();
	ParamDatabase& param_database = renderer.GetParamDatabase();

    std::string texture_file;

#ifdef BONFIRE_PLATFORM_WINDOWS
    {
        char exe_path[MAX_PATH];
        GetModuleFileNameA(NULL, exe_path, MAX_PATH);
        std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
        std::filesystem::path textures_dir = exe_dir / "Data/Resources/Textures";
        std::string file_buf = std::string(MAX_PATH, '\0');
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFile = (LPSTR)file_buf.c_str();
        ofn.nMaxFile = file_buf.size();
        ofn.lpstrInitialDir = textures_dir.string().c_str();
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.gif;*.tif;*.tiff;*.dds;*.hdr\0PNG Files\0*.png\0JPEG Files\0*.jpg;*.jpeg\0BMP Files\0*.bmp\0TGA Files\0*.tga\0All Files\0*.*\0";
        ofn.lpstrTitle = "Select texture file";
        if (!GetOpenFileNameA(&ofn))
        {
            Log::Info("File operation cancelled");
            return false;
        }
        file_buf.resize(file_buf.find('\0'));
        texture_file = file_buf;
    }
#elif defined(BONFIRE_PLATFORM_LINUX)
    {
        FILE* f = popen("zenity --file-selection --title=\"Select texture file\" --file-filter=\"Image Files | *.png *.jpg *.jpeg *.bmp *.tga *.tif *.tiff *.hdr\" 2>/dev/null", "r");
        if (!f) { Log::Info("File operation cancelled"); return false; }
        char buf[4096] = {};
        bool ok = fgets(buf, sizeof(buf), f) != nullptr;
        pclose(f);
        if (!ok || buf[0] == '\0') { Log::Info("File operation cancelled"); return false; }
        texture_file = buf;
        if (!texture_file.empty() && texture_file.back() == '\n') texture_file.pop_back();
    }
#endif

    std::filesystem::path absolute_path = texture_file;
    std::string abs_str = absolute_path.string();

    size_t data_pos = abs_str.find("Data");
    if (data_pos != std::string::npos)
    {
        default_diffuse_path = abs_str.substr(data_pos);
        std::replace(default_diffuse_path.begin(), default_diffuse_path.end(), '\\', '/');
    }
    else
        default_diffuse_path = texture_file;

    Log::Info("File selected at " + default_diffuse_path);

    uint32_t next_id = FIRST_ID;
    if (!scene.GetTextures().empty())
    {
        auto max_it = std::max_element(
            scene.GetTextures().begin(),
            scene.GetTextures().end(),
            [](const auto& a, const auto& b) { return a.first < b.first; }
            );
        next_id = max_it->first + 1;
    }

    std::filesystem::path path_obj(default_diffuse_path);
    std::string texture_name = path_obj.stem().string();

    std::shared_ptr<Texture> new_texture = std::make_shared<Texture>(default_diffuse_path, TextureType::DIFFUSE, false);
    new_texture->param_id = next_id;
    new_texture->name = texture_name;
    new_texture->Load();
    scene.GetTextures().insert_or_assign(next_id, new_texture);
    param_database.texture_params[next_id] = TextureParamData(texture_name, TextureType::DIFFUSE, false, default_diffuse_path);
    selected_texture_param_id = next_id;
    return true;
}

bool Editor::CreateAudioParam()
{
	Renderer& renderer = Engine::GetRenderer();
	Scene& scene = renderer.GetScene();
	ParamDatabase& param_database = renderer.GetParamDatabase();

    std::string audio_file;

#ifdef BONFIRE_PLATFORM_WINDOWS
    {
        char exe_path[MAX_PATH];
        GetModuleFileNameA(NULL, exe_path, MAX_PATH);
        std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
        std::filesystem::path audio_dir = exe_dir / "Data/Resources/Audio";
        std::string file_buf = std::string(MAX_PATH, '\0');
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFile = (LPSTR)file_buf.c_str();
        ofn.nMaxFile = file_buf.size();
        ofn.lpstrInitialDir = audio_dir.string().c_str();
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrFilter = "Audio Files\0*.wav;*.mp3;*.ogg;*.flac\0WAV Files\0*.wav\0MP3 Files\0*.mp3\0OGG Files\0*.ogg\0FLAC Files\0*.flac\0All Files\0*.*\0";
        ofn.lpstrTitle = "Select audio file";
        if (!GetOpenFileNameA(&ofn))
        {
            Log::Info("Audio file operation cancelled");
            return false;
        }
        file_buf.resize(file_buf.find('\0'));
        audio_file = file_buf;
    }
#elif defined(BONFIRE_PLATFORM_LINUX)
    {
        FILE* f = popen("zenity --file-selection --title=\"Select audio file\" --file-filter=\"Audio Files | *.wav *.mp3 *.ogg *.flac\" 2>/dev/null", "r");
        if (!f) { Log::Info("Audio file operation cancelled"); return false; }
        char buf[4096] = {};
        bool ok = fgets(buf, sizeof(buf), f) != nullptr;
        pclose(f);
        if (!ok || buf[0] == '\0') { Log::Info("Audio file operation cancelled"); return false; }
        audio_file = buf;
        if (!audio_file.empty() && audio_file.back() == '\n') audio_file.pop_back();
    }
#endif

    std::filesystem::path absolute_path = audio_file;
    std::string abs_str = absolute_path.string();
    std::string relative_audio_path;
    size_t data_pos = abs_str.find("Data");
    if (data_pos != std::string::npos)
    {
        relative_audio_path = abs_str.substr(data_pos);
        std::replace(relative_audio_path.begin(), relative_audio_path.end(), '\\', '/');
    }
    else
        relative_audio_path = audio_file;

    Log::Info("Audio file selected at " + relative_audio_path);
    uint32_t next_id = FIRST_ID;
    AudioSystem& audio_system = Engine::GetAudioManager();
    if (!audio_system.GetAudios().empty())
    {
        auto max_it = std::max_element(
            audio_system.GetAudios().begin(),
            audio_system.GetAudios().end(),
            [](const auto& a, const auto& b) { return a.first < b.first; }
        );
        next_id = max_it->first + 1;
    }

    std::filesystem::path path_obj(relative_audio_path);
    std::string audio_name = path_obj.stem().string();

    std::shared_ptr<Audio> new_audio = std::make_shared<Audio>(next_id, audio_name, relative_audio_path);
    audio_system.AddAudio(new_audio);
    param_database.audio_params.insert_or_assign(next_id, AudioParamData(audio_name, relative_audio_path));
    selected_audio_param_id = next_id;
    Log::Info("Loaded audio " + audio_name);
    return true;
}
