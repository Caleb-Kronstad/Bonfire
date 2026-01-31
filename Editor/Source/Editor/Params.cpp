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
    Renderer& renderer = Engine::GetRenderer();
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

    if (ImGui::BeginPopupContextItem("ModelParamPopupWindow"))
    {
        if (ImGui::MenuItem("Add New"))
        {
            CreateModelParam();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();	
    }

    ImGui::SameLine();

    ImGui::BeginChild("ModelDetails", ImVec2(0, -30), true);
    if (selected_model_param_id != 0 && param_database.model_params.contains(selected_model_param_id))
    {
        ModelParamData& model_data = param_database.model_params.at(selected_model_param_id);
        
        ImGui::Indent(8.0f);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(model_data.name.c_str()).x + input_text_padding);
        ImGui::InputText("##Name", &model_data.name); ImGui::SameLine(); ImGui::TextColored(highlight_secondary, std::to_string(selected_model_param_id).c_str());
        ImGui::Text(model_data.path.c_str());
        ImGui::Checkbox("Animated", &model_data.is_animated);

        ImGui::Spacing();
        if (scene.GetModels().contains(selected_model_param_id))
        {
            std::shared_ptr<Model> preview_model = scene.GetModels().at(selected_model_param_id);
            std::shared_ptr<Material> default_material = scene.GetMaterials().begin()->second;
            std::shared_ptr<Shader> preview_shader = scene.GetShaders().at(FIRST_ID + 1); // lit
            
            if (preview_model && default_material)
            {
                renderer.RenderModelPreview(preview_model, default_material, preview_shader, *model_preview_framebuffer, model_preview_rotation);
                ImGui::Spacing();
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
        ImGui::Checkbox("Rotate", &model_preview_auto_rotate_enabled);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Delete"))
        {
            if (selected_model_param_id != FIRST_ID && selected_model_param_id != FIRST_ID+1)
            {
                renderer.GetParamDatabase().model_params.erase(selected_model_param_id);
                selected_model_param_id = FIRST_ID;
            }
            else
                Log::Error("Cannot delete editor defaults");
        }
        
        ImGui::Unindent(8.0f);
    }
    ImGui::EndChild();
}

void Editor::DisplayTextureParams()
{
    Renderer& renderer = Engine::GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("TextureList", ImVec2(200, -30), true);
    for (auto& [texture_id, texture_data] : param_database.texture_params)
    {
        if (ImGui::Selectable(texture_data.name.c_str(), selected_texture_param_id == texture_id))
            selected_texture_param_id = texture_id;
    }
    ImGui::EndChild();

    if (ImGui::BeginPopupContextItem("TextureParamPopupWindow"))
    {
        if (ImGui::MenuItem("Add New"))
        {
            CreateTextureParam();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();	
    }

    ImGui::SameLine();

    ImGui::BeginChild("TextureDetails", ImVec2(0, -30), true);
    if (selected_texture_param_id != 0 && param_database.texture_params.contains(selected_texture_param_id))
    {
        auto& texture_data = param_database.texture_params[selected_texture_param_id];
        const char* texture_type_names[] = { "Diffuse", "Specular", "Normal", "Height", "Emission" };
        
        ImGui::Indent(8.0f);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(texture_data.name.c_str()).x + input_text_padding);
        ImGui::InputText("##Name", &texture_data.name); ImGui::SameLine(); ImGui::TextColored(highlight_secondary, std::to_string(selected_texture_param_id).c_str());
        ImGui::Text(texture_data.path.c_str());
        
        ImGui::Checkbox("Flip", &texture_data.flip);

        int current_index = static_cast<int>(texture_data.type);
        ImGui::SetNextItemWidth(200.0f);
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
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Delete"))
        {
            if (selected_texture_param_id > FIRST_ID + 4)
            {
                TextureType deleted_type = param_database.texture_params.at(selected_texture_param_id).type;
                uint32_t deleted_texture_id = selected_texture_param_id;

                renderer.GetParamDatabase().texture_params.erase(selected_texture_param_id);
                uint32_t reset_id = FIRST_ID + static_cast<uint32_t>(deleted_type);
                selected_texture_param_id = reset_id;

                for (auto& [mat_id, mat_data] : param_database.material_params)
                {
                    if (mat_data.diffuse_id == deleted_texture_id)
                        mat_data.diffuse_id = FIRST_ID;
                    if (mat_data.specular_id == deleted_texture_id)
                        mat_data.specular_id = FIRST_ID+1;
                    if (mat_data.normal_id == deleted_texture_id)
                        mat_data.normal_id = FIRST_ID+2;
                    if (mat_data.height_id == deleted_texture_id)
                        mat_data.height_id = FIRST_ID+3;
                    if (mat_data.emission_id == deleted_texture_id)
                        mat_data.emission_id = FIRST_ID+4;

                    if (scene.GetMaterials().contains(mat_id))
                    {
                        auto& material = scene.GetMaterials()[mat_id];
                        for (auto& texture : material->textures)
                        {
                            if (texture->param_id == deleted_texture_id)
                            {
                                uint32_t default_id = FIRST_ID + static_cast<uint32_t>(texture->type);
                                if (scene.GetTextures().contains(default_id))
                                    texture = scene.GetTextures()[default_id];
                            }
                        }
                    }
                }
            }
            else
                Log::Error("Cannot delete editor defaults");
        }
        
        ImGui::Unindent(8.0f);
    }
    ImGui::EndChild();
}

void Editor::DisplayMaterialParams()
{
    Renderer& renderer = Engine::GetRenderer();
    Scene& scene = renderer.GetScene();

    ImGui::BeginChild("MaterialList", ImVec2(200, -30), true);
    for (auto& [material_id, material_data] : scene.GetMaterials())
    {
        if (ImGui::Selectable(material_data->name.c_str(), selected_material_param_id == material_id))
        {
            selected_material_param_id = material_id;
        }
    }
    ImGui::EndChild();

    if (ImGui::BeginPopupContextItem("MaterialParamPopupWindow"))
    {
        if (ImGui::MenuItem("Add New"))
        {
            CreateMaterialParam();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();	
    }

    ImGui::SameLine();

    ImGui::BeginChild("MaterialDetails", ImVec2(0, -30), true);
    if (selected_material_param_id != 0 && scene.GetMaterials().contains(selected_material_param_id))
    {
        auto& material_data = scene.GetMaterials()[selected_material_param_id];

        ImGui::Indent(8.0f);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(material_data->name.c_str()).x + input_text_padding);
        ImGui::InputText("##Name", &material_data->name); ImGui::SameLine(); ImGui::TextColored(highlight_secondary, std::to_string(selected_material_param_id).c_str());
        ImGui::PushItemWidth(200.0f);
        ImGui::SliderFloat("Shininess", &material_data->shininess, 1.0f, 512.0f, "%.f");
        ImGui::SliderFloat2("Tiling", (float*)&material_data->texture_tiling, 1.0f, 100.0f, "%.f");
        ImGui::SliderFloat2("Offset", (float*)&material_data->texture_offset, 0.0f, 10.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::Spacing();
        if (scene.GetMaterials().contains(selected_material_param_id))
        {
            std::shared_ptr<Model> preview_model = scene.GetModels().at(FIRST_ID + 1); // sphere
            std::shared_ptr<Material> preview_material = scene.GetMaterials().at(selected_material_param_id);
            std::shared_ptr<Shader> preview_shader = scene.GetShaders().at(FIRST_ID + 1); // lit
            
            if (preview_model)
            {
                renderer.RenderModelPreview(preview_model, preview_material, preview_shader, *model_preview_framebuffer, model_preview_rotation);
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
        ImGui::Checkbox("Rotate", &model_preview_auto_rotate_enabled);
        
        ImGui::Spacing();
        for (auto& texture : material_data->textures)
        {
            ImGui::PushID(&texture);
            ImGui::Text(texture->name.c_str()); ImGui::SameLine(); ImGui::TextColored(highlight_secondary, std::to_string(texture->param_id).c_str());
            
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

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        if (ImGui::Button("Delete"))
        {
            if (selected_material_param_id != FIRST_ID)
            {
                renderer.GetParamDatabase().material_params.erase(selected_material_param_id);
                renderer.GetScene().GetMaterials().erase(selected_material_param_id);
                selected_material_param_id = FIRST_ID;
            
                for (auto& [id, entity] : scene.GetEntities())
                {
                    if (entity->HasComponent<ModelComponent>())
                    {
                        ModelComponent& model_component = entity->GetComponent<ModelComponent>();
                        if (model_component.material->param_id == selected_material_param_id)
                            model_component.material->param_id = FIRST_ID;
                    }
                }
            }
            else
                Log::Error("Cannot delete editor defaults");
        }
        
        ImGui::Unindent(8.0f);
    }
    ImGui::EndChild();
}

void Editor::DisplayShaderParams()
{
    Renderer& renderer = Engine::GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("ShaderList", ImVec2(200, 0), true);
    for (auto& [shader_id, shader_data] : param_database.shader_params)
    {
        if (ImGui::Selectable(shader_data.name.c_str(), selected_shader_param_id == shader_id))
        {
            selected_shader_param_id = shader_id;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ShaderDetails", ImVec2(0, 0), true);
    if (selected_shader_param_id != 0 && param_database.shader_params.contains(selected_shader_param_id))
    {
        auto& shader_data = param_database.shader_params[selected_shader_param_id];

        ImGui::Indent(8.0f);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(shader_data.name.c_str()).x + input_text_padding);
        ImGui::InputText("##Name", &shader_data.name); ImGui::SameLine(); ImGui::TextColored(highlight_secondary, std::to_string(selected_shader_param_id).c_str());
        ImGui::Text("Vertex Shader: %s", shader_data.vert_path.c_str());
        ImGui::Text("Fragment Shader: %s", shader_data.frag_path.c_str());
        ImGui::Text("Geometry Shader: %s", shader_data.geom_path.c_str());

        ImGui::Spacing();
        if (scene.GetShaders().contains(selected_shader_param_id))
        {
            std::shared_ptr<Model> preview_model = scene.GetModels().at(FIRST_ID + 1); // sphere
            std::shared_ptr<Material> default_material = scene.GetMaterials().begin()->second;
            std::shared_ptr<Shader> preview_shader = scene.GetShaders().at(selected_shader_param_id);
            
            if (preview_model)
            {
                renderer.RenderModelPreview(preview_model, default_material, preview_shader, *model_preview_framebuffer, model_preview_rotation);
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
        ImGui::Checkbox("Rotate", &model_preview_auto_rotate_enabled);

        ImGui::Unindent(8.0f);
    }
    ImGui::EndChild();
}

void Editor::DisplayAudioParams()
{
    Renderer& renderer = Engine::GetRenderer();
    Scene& scene = renderer.GetScene();
    ParamDatabase& param_database = renderer.GetParamDatabase();

    ImGui::BeginChild("AudioList", ImVec2(200, -30), true);
    for (auto& [audio_id, audio_data] : param_database.audio_params)
    {
        if (ImGui::Selectable(audio_data.name.c_str(), selected_audio_param_id == audio_id))
            selected_audio_param_id = audio_id;
    }
    ImGui::EndChild();

    if (ImGui::BeginPopupContextItem("AudioParamPopupWindow"))
    {
        if (ImGui::MenuItem("Add New"))
        {
            CreateAudioParam();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();	
    }

    ImGui::SameLine();

    ImGui::BeginChild("AudioDetails", ImVec2(0, -30), true);
    if (selected_audio_param_id != 0 && param_database.audio_params.contains(selected_audio_param_id))
    {
        auto& audio_data = param_database.audio_params[selected_audio_param_id];
        
        ImGui::Indent(8.0f);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(audio_data.name.c_str()).x + input_text_padding);
        ImGui::InputText("##Name", &audio_data.name); ImGui::SameLine(); ImGui::TextColored(highlight_secondary, std::to_string(selected_audio_param_id).c_str());
        ImGui::Text(audio_data.path.c_str());
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Delete"))
        {
            if (selected_audio_param_id != FIRST_ID)
            {
                renderer.GetParamDatabase().audio_params.erase(selected_audio_param_id);
                AudioSystem& audio_system = Engine::GetAudioSystem();
                audio_system.RemoveAudio(audio_system.GetAudio(selected_audio_param_id));
                selected_audio_param_id = FIRST_ID;
        
                for (auto& [id, entity] : scene.GetEntities())
                {
                    if (entity->HasComponent<ModelComponent>())
                    {
                        ModelComponent& model_component = entity->GetComponent<ModelComponent>();
                        if (model_component.material->param_id == selected_material_param_id)
                            model_component.material->param_id = FIRST_ID;
                    }
                }
            }
            else
                Log::Error("Cannot delete editor defaults");
        }
        
        ImGui::Unindent(8.0f);
    }
    ImGui::EndChild();
}