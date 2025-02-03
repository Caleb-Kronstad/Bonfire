#pragma once

#include "Core/Layer.h"

namespace Bonfire
{
	class EngineInterface : public Layer
	{
	public:
		EngineInterface();
		~EngineInterface();

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

		ImFont* m_InterfaceFont;
		bool m_InterfaceFontLoaded = false;

		GLuint m_FileImage = 0;
		int m_FileImageWidth = 0;
		int m_FileImageHeight = 0;

		int m_GizmoType = (int)ImGuizmo::TRANSLATE;
		int m_GizmoSpace = 0;
	};
}
