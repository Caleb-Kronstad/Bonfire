#pragma once

#include "bonfire_pch.h"

namespace Bonfire
{
	enum class InputType
	{
		None = 0,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	class Input
	{
	public:
		virtual ~Input() = default;

		bool Handled = false;

		virtual InputType GetInputType() const = 0;
		virtual std::string ToString() const { return ""; }
	};
}