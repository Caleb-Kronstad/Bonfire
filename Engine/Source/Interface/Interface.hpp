#pragma once

#include "Core/Layer.hpp"
#include "Core/Utility.hpp"

namespace Bonfire
{
	class Interface : public Layer
	{
	public:
		Interface();
		~Interface();
		
		void Begin();
		void End();
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnInput(Input& input) override;

		void SetMainStyle();

	public:
		ImVec4 text_primary;
		ImVec4 background_primary;
		ImVec4 background_secondary;
		ImVec4 background_tertiary;
		ImVec4 highlight_primary;
		ImVec4 highlight_secondary;

	private:
		bool block_inputs = false;

		GLuint file_image = 0;
		int file_image_width = 0;
		int file_image_height = 0;

		int gizmo_type = (int)ImGuizmo::TRANSLATE;
		int gizmo_space = 0;
	};
}