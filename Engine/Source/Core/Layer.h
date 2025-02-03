#pragma once

#include "Input/Input.h"
#include "Input/InputCodes.h"
#include "Input/InputTypes.h"

namespace Bonfire
{
	class Layer
	{
	protected:
		std::string m_DebugName;

	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate();
		virtual void OnInput(Input& input);

		std::string GetName() const { return m_DebugName; }
	};
}