#pragma once

#include "Core/Layer.h"

#include "Renderer/Mesh.h"

#include "Input/Input.h"
#include "Input/InputCodes.h"
#include "Input/InputTypes.h"

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