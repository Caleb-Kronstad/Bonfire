#pragma once

#include "Input/Input.hpp"
#include "Input/InputCodes.hpp"
#include "Input/InputTypes.hpp"

namespace Bonfire
{
	class Layer
	{
	public:
		Layer(std::string debug_name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate();
		virtual void OnInterfaceUpdate();
		virtual void OnInput(Input& input);

		std::string GetName() const { return debug_name; }
		
	protected:
		std::string debug_name;
	};
}