#pragma once

#include "Core/Layer.hpp"

namespace Bonfire
{
	class Interface : public Layer
	{
	public:
		Interface();
		~Interface();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnInput(Input& input) override;

		void SetCustomStyle();
		void SetClearStyle();

	private:
		void Begin();
		void End();

	public:

	private:
		bool m_BlockInputs = false;

		GLuint m_FileImage = 0;
		int m_FileImageWidth = 0;
		int m_FileImageHeight = 0;

		int m_GizmoType = (int)ImGuizmo::TRANSLATE;
		int m_GizmoSpace = 0;
	};
}
