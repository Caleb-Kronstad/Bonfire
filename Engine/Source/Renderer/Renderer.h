#pragma once

#include "Core/Layer.h"

namespace Bonfire
{
	class Renderer : public Layer
	{
	public:
		Renderer();
		~Renderer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnInput(Input& input) override;

	private:

	};
}