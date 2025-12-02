#include "Editor.hpp"

void Editor::DrawParamEditor()
{
    ImGui::PushFont(editor_font);
    ImGui::Begin("Params", nullptr);
    DrawActiveTitleLine(highlight_primary, background_tertiary);
    ImGui::Indent(8.0f);
    ImGui::Spacing();
    
    if (ImGui::BeginTabBar("ParamEditorTabs"))
    {
        ImGui::PushStyleColor(ImGuiCol_TabActive, highlight_primary);
        ImGui::PushStyleColor(ImGuiCol_TabHovered, highlight_secondary);
        ImGui::PushStyleColor(ImGuiCol_Tab, background_tertiary);
        ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, highlight_primary);
        
        if (ImGui::BeginTabItem("Model Params"))
        {
            DisplayModelParams();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Texture Params"))
        {
            DisplayTextureParams();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Material Params"))
        {
            DisplayMaterialParams();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Shader Params"))
        {
            DisplayShaderParams();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Audio Params"))
        {
            DisplayAudioParams();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
        ImGui::PopStyleColor(4);
    }
    
    ImGui::PopFont();
    ImGui::Unindent(8.0f);
    ImGui::End();
}

void Editor::DisplayModelParams()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = project.GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("ModelList", ImVec2(200, -30), true);
    for (auto& [model_id, model_data] : param_database.model_params)
    {
        if (ImGui::Selectable(model_data.name.c_str(), selected_model_param_id == model_id))
        {
            model_preview_rotation = glm::vec3(0.0f);
            selected_model_param_id = model_id;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ModelDetails", ImVec2(0, -30), true);
    if (selected_model_param_id != 0 && param_database.model_params.contains(selected_model_param_id))
    {
        auto& model_data = param_database.model_params[selected_model_param_id];
        ImGui::Text("ID: %u", selected_model_param_id);
        ImGui::Text("Path: %s", model_data.path.c_str());
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("Name", &model_data.name);
        ImGui::Checkbox("Animated", &model_data.is_animated);

        if (scene.GetModels().contains(selected_model_param_id))
        {
            std::shared_ptr<Model> preview_model = scene.GetModels().at(selected_model_param_id);
            std::shared_ptr<Material> default_material = default_textures.at(0) ? scene.GetMaterials().begin()->second : nullptr;

            if (preview_model && default_material)
            {
                renderer.RenderModelPreview(preview_model, default_material, *model_preview_framebuffer, model_preview_rotation);
                ImGui::Spacing();
                
                ImVec2 preview_pos = ImGui::GetCursorScreenPos();
                ImGui::Image((void*)(intptr_t)model_preview_framebuffer->GetColorAttachment(), ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
                
                if (ImGui::IsItemHovered())
                {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        model_preview_is_dragging = true;
                        model_preview_auto_rotate = false;
                        ImVec2 mouse_pos = ImGui::GetMousePos();
                        model_preview_last_mouse_pos = glm::vec2(mouse_pos.x, mouse_pos.y);
                    }
                }

                if (model_preview_is_dragging)
                {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    {
                        ImVec2 current_mouse_pos = ImGui::GetMousePos();
                        glm::vec2 mouse_delta = glm::vec2(current_mouse_pos.x, current_mouse_pos.y) - model_preview_last_mouse_pos;
                        
                        model_preview_rotation.y += mouse_delta.x * 0.5f;
                        model_preview_rotation.x += mouse_delta.y * 0.5f;
                        
                        if (model_preview_rotation.x >= 360.0f)
                            model_preview_rotation.x -= 360.0f;
                        if (model_preview_rotation.x < 0.0f)
                            model_preview_rotation.x += 360.0f;
                        
                        if (model_preview_rotation.y >= 360.0f)
                            model_preview_rotation.y -= 360.0f;
                        if (model_preview_rotation.y < 0.0f)
                            model_preview_rotation.y += 360.0f;
                        
                        model_preview_last_mouse_pos = glm::vec2(current_mouse_pos.x, current_mouse_pos.y);
                    }
                    else
                    {
                        model_preview_is_dragging = false;
                        model_preview_auto_rotate = true;
                    }
                }
            }
        }
    }
    ImGui::EndChild();

    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
    std::filesystem::path models_dir = exe_dir / "Data/Resources/Models";
    std::string model_file = std::string(MAX_PATH, '\0');

    if (ImGui::Button("+"))
    {
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFile = (LPSTR)model_file.c_str();
        ofn.nMaxFile = model_file.size();
        ofn.lpstrInitialDir = models_dir.string().c_str();
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrFilter = "Model Files\0*.obj;*.fbx;*.dae;*.gltf;*.glb\0Obj Files\0*.obj\0FBX Files\0*.fbx\0DAE Files\0*.dae\0glTF Files\0*.gltf;*.glb\0All Files\0*.*\0";
        ofn.lpstrTitle = "Select model file";

        if (GetOpenFileNameA(&ofn))
        {
            model_file.resize(model_file.find('\0'));

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
        
            uint32_t next_id = 1000;
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
        }
        else
            Log::Info("File operation cancelled");
    }
}

void Editor::DisplayTextureParams()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = project.GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("TextureList", ImVec2(200, -30), true);
    for (auto& [texture_id, texture_data] : param_database.texture_params)
    {
        if (ImGui::Selectable(texture_data.name.c_str(), selected_texture_param_id == texture_id))
            selected_texture_param_id = texture_id;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("TextureDetails", ImVec2(0, -30), true);
    if (selected_texture_param_id != 0 && param_database.texture_params.contains(selected_texture_param_id))
    {
        auto& texture_data = param_database.texture_params[selected_texture_param_id];
        const char* texture_type_names[] = { "Diffuse", "Specular", "Normal", "Height", "Emission" };
        
        ImGui::Text("ID: %u", selected_texture_param_id);
        ImGui::Text("Path: %s", texture_data.path.c_str());
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("Name", &texture_data.name);
        
        ImGui::Checkbox("Flip", &texture_data.flip);
        
        int current_index = static_cast<int>(texture_data.type);
        if (ImGui::BeginCombo("Type", texture_type_names[current_index]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(texture_type_names); n++)
            {
                bool is_selected = (current_index == n);
                if (ImGui::Selectable(texture_type_names[n], is_selected))
                {
                    texture_data.type = static_cast<TextureType>(n);
                }
                
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (scene.GetTextures().contains(selected_texture_param_id))
        {
            GLuint texture_gl_id = scene.GetTextures().at(selected_texture_param_id)->gl_id;
            ImGui::Image((void*)texture_gl_id, ImVec2(100, 100));
        }
    }
    ImGui::EndChild();

    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
    std::filesystem::path textures_dir = exe_dir / "Data/Resources/Textures";
    std::string texture_file = std::string(MAX_PATH, '\0');
    
    if (ImGui::Button("+"))
    {
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFile = (LPSTR)texture_file.c_str();
        ofn.nMaxFile = texture_file.size();
        ofn.lpstrInitialDir = textures_dir.string().c_str();
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.gif;*.tif;*.tiff;*.dds;*.hdr\0PNG Files\0*.png\0JPEG Files\0*.jpg;*.jpeg\0BMP Files\0*.bmp\0TGA Files\0*.tga\0All Files\0*.*\0";
        ofn.lpstrTitle = "Select texture file";

        if (GetOpenFileNameA(&ofn))
        {
            texture_file.resize(texture_file.find('\0'));

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
        
            uint32_t next_id = 200001;
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
        }
        else
            Log::Info("File operation cancelled");
    }
}

void Editor::DisplayMaterialParams()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = project.GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("MaterialList", ImVec2(200, -30), true);
    for (auto& [material_id, material_data] : scene.GetMaterials())
    {
        if (ImGui::Selectable(material_data->name.c_str(), selected_material_param_id == material_id))
            selected_material_param_id = material_id;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("MaterialDetails", ImVec2(0, -30), true);
    if (selected_material_param_id != 0 && scene.GetMaterials().contains(selected_material_param_id))
    {
        auto& material_data = scene.GetMaterials()[selected_material_param_id];
        
        ImGui::Text("ID: %u", selected_material_param_id);
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("Name", &material_data->name);
        ImGui::SliderFloat("Shininess", &material_data->shininess, 1.0f, 512.0f, "%.f");
        ImGui::SliderFloat2("Tiling", (float*)&material_data->texture_tiling, 1.0f, 100.0f, "%.f");
        ImGui::SliderFloat2("Offset", (float*)&material_data->texture_offset, 0.1f, 10.0f, "%.2f");

        ImGui::Spacing();
        ImGui::Text("Textures");
        for (auto& texture : material_data->textures)
        {
            ImGui::PushID(&texture);
            ImGui::Text("%s (ID: %u)", texture->name.c_str(), texture->param_id);
            
            if (ImGui::ImageButton((void*)texture->gl_id, ImVec2(100, 100)))
            {
                ImGui::OpenPopup("ChangeMaterialTexture");
            }
            if (ImGui::BeginPopup("ChangeMaterialTexture"))
            {
                for (auto& [texture_id, list_texture] : scene.GetTextures())
                {
                    if (texture->type != list_texture->type) continue;
                    ImGui::PushID(&texture_id);
                    ImGui::Image((void*)(intptr_t)list_texture->gl_id, ImVec2(20, 20));
                    ImGui::SameLine();
                    if (ImGui::Selectable(list_texture->name.c_str(), false, 0, ImVec2(150, 20)))
                    {
                        texture = list_texture;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::PopID();
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("+"))
    {
        uint32_t next_id = 1000;
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
    }
}

void Editor::DisplayShaderParams()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = project.GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("ShaderList", ImVec2(200, 0), true);
    for (auto& [shader_id, shader_data] : param_database.shader_params)
    {
        if (ImGui::Selectable(shader_data.name.c_str(), selected_shader_param_id == shader_id))
            selected_shader_param_id = shader_id;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ShaderDetails", ImVec2(0, 0), true);
    if (selected_shader_param_id != 0 && param_database.shader_params.contains(selected_shader_param_id))
    {
        auto& shader_data = param_database.shader_params[selected_shader_param_id];
        
        ImGui::Text("ID: %u", selected_shader_param_id);
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("Name", &shader_data.name);
        ImGui::Text("Vertex Shader: %s", shader_data.vert_path.c_str());
        ImGui::Text("Fragment Shader: %s", shader_data.frag_path.c_str());
        ImGui::Text("Geometry Shader: %s", shader_data.geom_path.c_str());
    }
    ImGui::EndChild();
}

void Editor::DisplayAudioParams()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = project.GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("AudioList", ImVec2(200, -30), true);
    for (auto& [audio_id, audio_data] : param_database.audio_params)
    {
        if (ImGui::Selectable(audio_data.name.c_str(), selected_audio_param_id == audio_id))
            selected_audio_param_id = audio_id;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("AudioDetails", ImVec2(0, -30), true);
    if (selected_audio_param_id != 0 && param_database.audio_params.contains(selected_audio_param_id))
    {
        auto& audio_data = param_database.audio_params[selected_audio_param_id];
        
        ImGui::Text("ID: %u", selected_audio_param_id);
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("Name", &audio_data.name);
        ImGui::Text("Path: %s", audio_data.path.c_str());
    }
    ImGui::EndChild();

    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
    std::filesystem::path audio_dir = exe_dir / "Data/Resources/Audio";
    std::string audio_file = std::string(MAX_PATH, '\0');

    if (ImGui::Button("+"))
    {
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFile = (LPSTR)audio_file.c_str();
        ofn.nMaxFile = audio_file.size();
        ofn.lpstrInitialDir = audio_dir.string().c_str();
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrFilter = "Audio Files\0*.wav;*.mp3;*.ogg;*.flac\0WAV Files\0*.wav\0MP3 Files\0*.mp3\0OGG Files\0*.ogg\0FLAC Files\0*.flac\0All Files\0*.*\0";
        ofn.lpstrTitle = "Select audio file";

        if (GetOpenFileNameA(&ofn))
        {
            audio_file.resize(audio_file.find('\0'));
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
            uint32_t next_id = 1000;
            AudioSystem& audio_system = Project::GetAudioSystem();
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
        }
        else
            Log::Info("Audio file operation cancelled");
    }
}