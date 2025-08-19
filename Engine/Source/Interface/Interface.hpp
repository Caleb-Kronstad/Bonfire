#pragma once

#include "Core/Layer.hpp"

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

		void SetCustomStyle();
		void SetClearStyle();

	public:

	private:
		bool block_inputs = false;

		GLuint file_image = 0;
		int file_image_width = 0;
		int file_image_height = 0;

		int gizmo_type = (int)ImGuizmo::TRANSLATE;
		int gizmo_space = 0;
	};
}
