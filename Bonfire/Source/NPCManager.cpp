#include "NPCManager.hpp"

NPCManager::NPCManager()
{
    
}
NPCManager::~NPCManager()
{
    
}

void NPCManager::OnAttach()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = Project::GetRenderer();
    PhysicsSystem& physics_system = Project::GetPhysicsSystem();
    AudioSystem& audio_system = Project::GetAudioSystem();
    ScriptSystem& script_system = Project::GetScriptSystem();
    Scene& scene = renderer.GetScene();

    player_entity = scene.GetEntityOfName("Player");
    knight.entity = scene.GetEntityOfName("Knight");
    merchant.entity = scene.GetEntityOfName("Merchant");
    knight.interact_range = 10.0f;
    merchant.interact_range = 15.0f;
    
    for (auto& layer : project.GetLayers())
    {
        if (Player* player_ptr = dynamic_cast<Player*>(layer.get()))
        {
            player_layer = player_ptr;
            break;
        }
    }

    if (!player_layer || !player_entity) return;

    if (knight.entity != nullptr)
    {
        if (knight.entity->HasComponent<AnimationComponent>())
        {
            AnimationComponent& animation_component = knight.entity->GetComponent<AnimationComponent>();
            animation_component.animator->Play("Armature|HMM");
        }
    }
    if (merchant.entity != nullptr)
    {
        if (merchant.entity->HasComponent<AnimationComponent>())
        {
            AnimationComponent& animation_component = merchant.entity->GetComponent<AnimationComponent>();
            animation_component.animator->Play("Armature|IDLE");
            animation_component.animator->loop = true;
        }
    }
    
    // find and activate all poses for petrified npcs
    PetrifiedNPC petrified_npc_01 = PetrifiedNPC(scene.GetEntityOfName("PetrifiedVillager01"), "Armature|FROZEN01");
    PetrifiedNPC petrified_npc_02 = PetrifiedNPC(scene.GetEntityOfName("PetrifiedVillager02"), "Armature|FROZEN02");
    PetrifiedNPC petrified_npc_03 = PetrifiedNPC(scene.GetEntityOfName("PetrifiedVillager03"), "Armature|FROZEN03");
    PetrifiedNPC petrified_npc_04 = PetrifiedNPC(scene.GetEntityOfName("PetrifiedVillager04"), "Armature|FROZEN02");
    PetrifiedNPC petrified_npc_05 = PetrifiedNPC(scene.GetEntityOfName("PetrifiedVillager05"), "Armature|FROZEN02");
    PetrifiedNPC petrified_npc_06 = PetrifiedNPC(scene.GetEntityOfName("PetrifiedVillager06"), "Armature|FROZEN03");
    PetrifiedNPC petrified_npc_07 = PetrifiedNPC(scene.GetEntityOfName("PetrifiedVillager07"), "Armature|FROZEN03");
    petrified_npcs.push_back(petrified_npc_01);
    petrified_npcs.push_back(petrified_npc_02);
    petrified_npcs.push_back(petrified_npc_03);
    petrified_npcs.push_back(petrified_npc_04);
    petrified_npcs.push_back(petrified_npc_05);
    petrified_npcs.push_back(petrified_npc_06);
    petrified_npcs.push_back(petrified_npc_07);

    for (PetrifiedNPC& npc : petrified_npcs)
    {
        if (!npc.entity) continue;
        if (npc.entity->HasComponent<AnimationComponent>())
            npc.entity->GetComponent<AnimationComponent>().animator->Play(npc.animation);
    }

    shop_items = {
        {"Test Item 1", "This is a test item", 1},
        {"Test Item 2", "This is a test item", 10},
        {"Test Item 3", "This is a test item", 4},
        {"Test Item 4", "This is a test item", 155},
        {"Test Item 5", "This is a test item", 2},
        {"Test Item 6", "This is a test item", 161236}
    };
}
void NPCManager::OnDetach()
{
    if (!player_entity) return;

    if (knight.entity != nullptr)
    {
        if (knight.entity->HasComponent<AnimationComponent>())
            knight.entity->GetComponent<AudioComponent>().audio->Stop();
    }
    if (merchant.entity != nullptr)
    {
        if (merchant.entity->HasComponent<AnimationComponent>())
            merchant.entity->GetComponent<AudioComponent>().audio->Stop();
    }
}

void NPCManager::OnUpdate(const float& delta_time)
{
    if (!player_entity) return;
    
    Project& project = Project::GetInstance();
    Renderer& renderer = Project::GetRenderer();
    PhysicsSystem& physics_system = Project::GetPhysicsSystem();
    AudioSystem& audio_system = Project::GetAudioSystem();
    ScriptSystem& script_system = Project::GetScriptSystem();
    Scene& scene = renderer.GetScene();

    if (knight.entity != nullptr)
    {
        float knight_to_player = glm::distance(knight.entity->position, player_entity->position);
        if (knight_to_player < knight.interact_range)
            knight.can_interact = true;
        else
            knight.can_interact = false;
    }
    if (merchant.entity != nullptr)
    {
        float merchant_to_player = glm::distance(merchant.entity->position, player_entity->position);
        merchant.can_interact = merchant_to_player < merchant.interact_range;
    }
}

void NPCManager::OnInput(Input& input)
{
    Project& project = Project::GetInstance();
    Window& window = project.GetWindow();
    
    if (!player_entity) return;

    if (input.GetInputType() == InputType::KeyPressed)
    {
        const auto key_input = dynamic_cast<KeyPressedInput&>(input);
        
        if (key_input.GetKeyCode() == InputCode::F)
        {
            if (knight.entity != nullptr && knight.can_interact && !knight.interacting)
            {
                knight.interacting = true;
                knight.interaction_count++;
                
                if (knight.entity->HasComponent<AudioComponent>())
                    knight.entity->GetComponent<AudioComponent>().audio->Play();
            }
            
            if (merchant.entity != nullptr && merchant.can_interact && !merchant.interacting)
            {
                merchant.interacting = true;
                displaying_shop = true;

                player_layer->camera_can_move = false;
                player_entity->GetComponent<CameraComponent>().camera->LookAt(merchant.entity->position);
                glm::vec3 velocity = player_entity->GetComponent<PhysicsComponent>().physics_body->GetLinearVelocity();
                player_entity->GetComponent<PhysicsComponent>().physics_body->SetLinearVelocity(glm::vec3(0.0f, velocity.y, 0.0f));
                
                if (merchant.entity->HasComponent<AnimationComponent>())
                {
                    AnimationComponent& animation_component = merchant.entity->GetComponent<AnimationComponent>();
                    if (merchant.interaction_count == 0)
                    {
                        if (merchant.entity->HasComponent<AudioComponent>())
                            merchant.entity->GetComponent<AudioComponent>().audio->Play();
                        
                        animation_component.animator->Play("Armature|GREET");
                        animation_component.animator->loop = false;
                        merchant.interaction_count++;
                    }
                }

                glfwSetInputMode(window.GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                player_layer->player_stats.move_speed = 0.0f;
            }
        }
    }
}

void NPCManager::OnInterfaceUpdate()
{
    if (!player_entity) return;

    if (displaying_shop && merchant.entity != nullptr)
        DisplayMerchantShop();
}

void NPCManager::DisplayMerchantShop()
{
    Project& project = Project::GetInstance();
    Window& window = project.GetWindow();
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImVec2 window_size(400, 500);
    ImVec2 screen_size = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2((screen_size.x - window_size.x) * 0.1f, (screen_size.y - window_size.y) * 0.5f));
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(25/255.0f, 25/255.0f, 25/255.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(25/255.0f, 25/255.0f, 25/255.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(35/255.0f, 35/255.0f, 35/255.0f, 1.0f));

    ImGui::Begin("Shop", nullptr, window_flags);

    for (const auto& item : shop_items)
    {
        ImGui::PushID(&item);
        ImGui::Text(item.name.c_str());
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), item.description.c_str());
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), std::to_string(item.price).c_str());

        if (ImGui::Button("Purchase", ImVec2(60, 25)))
        {
            // buy item
        }

        ImGui::Separator();
        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetCursorPosX((window_size.x - 100.0f) * 0.5f);
    if (ImGui::Button("Close", ImVec2(100, 30)))
    {
        merchant.interacting = false;
        displaying_shop = false;
        
        player_layer->camera_can_move = true;
        player_layer->player_stats.move_speed = player_layer->default_move_speed;
        player_entity->GetComponent<CameraComponent>().camera->IsFirstMouse() = true;

        AudioSystem& audio_system = Project::GetAudioSystem();
        AudioComponent& audio_component = merchant.entity->GetComponent<AudioComponent>();
        if (!audio_component.audio->IsPlaying())
        {
            audio_component.audio = audio_system.GetAudio(1012);
            audio_component.audio->SetVolume(5.0f);
            audio_component.audio->Play();
        
            AnimationComponent& animation_component = merchant.entity->GetComponent<AnimationComponent>();
            animation_component.animator->Play("Armature|IDLE");
            animation_component.animator->loop = true;
        }

        glfwSetInputMode(window.GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    ImGui::PopStyleColor(3);
    ImGui::End();
}