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

	private:

	};
}
