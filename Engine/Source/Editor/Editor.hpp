#pragma once

#include "Core/Layer.hpp"
#include "Core/Utility.hpp"

#include "Editor/ConsoleCapture.hpp"

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Ray.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/SkeletalModel.hpp"
#include "Renderer/Entity.hpp"
#include "Renderer/Scene.hpp"

#include "Input/Input.hpp"
#include "Input/InputCodes.hpp"
#include "Input/InputTypes.hpp"

namespace Bonfire
{
    inline bool CTRL_DOWN = false;
    
    class Editor : Layer
    {
    public:
        Editor(const std::string& config_path);
        ~Editor();
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(const float& delta_time) override;
        void OnInterfaceUpdate() override;
        void OnInput(Input& input) override;

        void OnInterfaceEndUpdate();
        void UpdateInterfaceStyle();

        glm::vec4 GetBackgroundColor() const { return RgbaToGlmVec4(background_secondary.x, background_secondary.y, background_secondary.z); }
        Entity& GetSelectedEntity() { return *selected_entity; }
        Camera& GetEngineCamera() { return *engine_camera; }

        bool PreviewAnimations() const { return preview_animations; }
        bool EditorViewportVisible() const { return editor_viewport_visible; }
        bool ProjectViewportVisible() const { return project_viewport_visible; }

        bool LoadEditorConfig();
        bool SaveEditorConfig();

    private:
        void SetInterfaceStyle();

        void DrawMenuBar();
        void DrawEditorViewport();
        void DrawProjectViewport();
        void DrawDebugInfo();
        void DrawToolbar();
        void DrawProjectSettings();
        void DrawConsole();
        void DrawHierarchy();
        void DrawDetails();
        void DrawParamEditor();

        void DrawActiveTitleLine(const ImVec4& active_color, const ImVec4& inactive_color, float thickness = 3.0f);
        void DrawEntityTree(std::shared_ptr<Entity> entity);
        void CreateEntity(std::shared_ptr<Entity> parent = nullptr);
        void DuplicateEntity(std::shared_ptr<Entity> entity);
        void DeleteEntity(std::shared_ptr<Entity> entity);
        bool IsDescendentOf(std::shared_ptr<Entity> potential_child, std::shared_ptr<Entity> potential_parent);
        void ReparentEntity(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> new_parent);

        void CreateModelComponent();
        void CreateLightSourceComponent();
        void CreatePhysicsComponent();
        void CreateAnimationComponent();
        void CreateAudioComponent();

    private:
		std::string project_path;
        std::string config_path;
        std::string serialized_scene_data;

        bool preview_animations = true;
        
        std::shared_ptr<Entity> selected_entity;
        std::shared_ptr<Entity> entity_to_create;
        std::shared_ptr<Entity> entity_to_delete;
        std::shared_ptr<Entity> entity_to_reparent;
        std::shared_ptr<Entity> reparent_target;

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
        bool project_viewport_focused = false;
        bool project_viewport_hovered = false;
        bool project_viewport_visible = false;

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
        
        // CUSTOMIZATION
        //ImFont* font_title;
        //ImFont* font_body;
        ImVec4 text_primary;
        ImVec4 background_primary;
        ImVec4 background_secondary;
        ImVec4 background_tertiary;
        ImVec4 highlight_primary;
        ImVec4 highlight_secondary;
    };
}
