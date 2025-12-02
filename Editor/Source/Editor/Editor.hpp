#pragma once

#include "BonfireEngine.hpp"

using namespace Bonfire;

inline bool CTRL_DOWN = false;

class Editor : public Layer
{
public:
    Editor(const std::string& config_path);
    ~Editor();
        
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const float& delta_time) override;
    void OnInterfaceUpdate() override;
    void OnInput(Input& input) override;

    void UpdateEditorInterfaceStyle();

    Entity& GetSelectedEntity() { return *selected_entity; }
    Camera& GetEngineCamera() { return *engine_camera; }

    bool PreviewAnimations() const { return preview_animations; }
    bool PreviewAudios() const { return preview_audios; }
    bool EditorViewportVisible() const { return editor_viewport_visible; }

    bool LoadEditorConfig();
    bool SaveEditorConfig();

private:
    void SetInterfaceStyle();

    void CreateEntity(std::shared_ptr<Entity> parent = nullptr);
    void CreateModelComponent(std::shared_ptr<Entity> entity);
    void CreateLightSourceComponent(std::shared_ptr<Entity> entity);
    void CreatePhysicsComponent(std::shared_ptr<Entity> entity);
    void CreateAnimationComponent(std::shared_ptr<Entity> entity);
    void CreateAudioComponent(std::shared_ptr<Entity> entity);
    void CreateScriptComponent(std::shared_ptr<Entity> entity);
    void CreateCameraComponent(std::shared_ptr<Entity> entity);

    void RemoveItems();
    void RemoveEntity(std::shared_ptr<Entity> entity);
    void RemoveModelComponent(std::shared_ptr<Entity> entity);
    void RemoveLightSourceComponent(std::shared_ptr<Entity> entity);
    void RemovePhysicsComponent(std::shared_ptr<Entity> entity);
    void RemoveAnimationComponent(std::shared_ptr<Entity> entity);
    void RemoveAudioComponent(std::shared_ptr<Entity> entity);
    void RemoveScriptComponent(std::shared_ptr<Entity> entity);
    void RemoveCameraComponent(std::shared_ptr<Entity> entity);

    void DuplicateEntity(std::shared_ptr<Entity> entity);
    void DuplicateModelComponent(std::shared_ptr<Entity> entity);
    void DuplicateLightSourceComponent(std::shared_ptr<Entity> entity);
    void DuplicatePhysicsComponent(std::shared_ptr<Entity> entity);
    void DuplicateAnimationComponent(std::shared_ptr<Entity> entity);
    void DuplicateAudioComponent(std::shared_ptr<Entity> entity);
    void DuplicateScriptComponent(std::shared_ptr<Entity> entity);
    void DuplicateCameraComponent(std::shared_ptr<Entity> entity);

    void DisplayModelComponent();
    void DisplayLightSourceComponent();
    void DisplayPhysicsComponent();
    void DisplayAnimationComponent();
    void DisplayAudioComponent();
    void DisplayCameraComponent();
    void DisplayScriptComponent();

    void DisplayModelParams();
    void DisplayTextureParams();
    void DisplayMaterialParams();
    void DisplayShaderParams();
    void DisplayAudioParams();
    
    bool IsDescendentOf(std::shared_ptr<Entity> potential_child, std::shared_ptr<Entity> potential_parent);
    void ReparentEntity(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> new_parent);

    void DrawMenuBar();
    void DrawEditorViewport();
    void DrawProjectViewport(ImGuiWindowFlags window_flags);
    void DrawDebugInfo();
    void DrawToolbar();
    void DrawProjectSettings();
    void DrawConsole();
    void DrawHierarchy();
    void DrawDetails();
    void DrawParamEditor();

    void DrawActiveTitleLine(const ImVec4& active_color, const ImVec4& inactive_color, float thickness = 3.0f);
    void DrawEntityTree(std::shared_ptr<Entity> entity);

    void BuildProject();

    void ExecuteCommand(std::unique_ptr<Command> command);
    CommandHistory& GetCommandHistory() { return *command_history; }

private:
	std::string project_path;
    std::string config_path;
    std::string serialized_scene_data;

    std::unique_ptr<CommandHistory> command_history;
    uint8_t undo_redo_steps = 64;
    bool is_modifying_transform = false;
    glm::vec3 transform_start_position = glm::vec3(0.0f);
    glm::vec3 transform_start_rotation = glm::vec3(0.0f);
    glm::vec3 transform_start_scale = glm::vec3(1.0f);
    
    std::unique_ptr<Framebuffer> editor_viewport_framebuffer;
    glm::vec2 editor_viewport_size = { 1280, 720 };

    bool preview_animations = true;
    bool preview_audios = true;
    
    std::shared_ptr<Entity> selected_entity = nullptr;
    std::shared_ptr<Entity> selected_entity_to_remove_components = nullptr;
    std::shared_ptr<Entity> entity_to_create = nullptr;
    std::shared_ptr<Entity> entity_to_remove = nullptr;
    std::shared_ptr<Entity> entity_to_reparent = nullptr;
    std::shared_ptr<Entity> reparent_target = nullptr;

    std::shared_ptr<Material> selected_material_param = nullptr;
    ModelParamData* selected_model_param = nullptr;
    TextureParamData* selected_texture_param = nullptr;
    ShaderParamData* selected_shader_param = nullptr;
    AudioParamData* selected_audio_param = nullptr;
    
    ComponentType component_to_remove = ComponentType::UNKNOWN;

    std::unique_ptr<Camera> engine_camera;
    float engine_camera_speed = 5.0f;
    float engine_camera_turn_sensitivity = 0.1f;
    bool engine_camera_constrain_pitch = true;
    bool engine_camera_can_turn = false;
    bool engine_camera_can_move = true;

    std::unique_ptr<ConsoleCapture> console_capture;
    float drag_step = 1.0f;
    int gizmo_type = ImGuizmo::TRANSLATE;
    int gizmo_space = 0;
    bool block_interface_inputs = false;

    bool editor_viewport_focused = false;
    bool editor_viewport_hovered = false;
    bool editor_viewport_visible = true;

    std::unique_ptr<Texture> play_icon;
    std::unique_ptr<Texture> move_icon;
    std::unique_ptr<Texture> rotate_icon;
    std::unique_ptr<Texture> resize_icon;
    
    std::string default_model_path;
    std::string default_diffuse_path;
    std::string default_specular_path;
    std::string default_normal_path;
    std::string default_height_path;
    std::string default_emission_path;
    std::string default_shader_vert_path;
    std::string default_shader_frag_path;
    std::string default_shader_geom_path;

    std::array<std::shared_ptr<Texture>, 5> default_textures;
    float default_texture_shininess = 64.0f;

    int view = 0;
    
    // CUSTOMIZATION
    ImFont* editor_font = nullptr;
    ImVec4 text_primary;
    ImVec4 background_primary;
    ImVec4 background_secondary;
    ImVec4 background_tertiary;
    ImVec4 highlight_primary;
    ImVec4 highlight_secondary;
};