#pragma once

#include "bonfire_pch.h"
#include "Input.h"
#include "InputCodes.h"

namespace Bonfire
{

	// KEYBOARD

	class KeyInput : public Input
	{
	protected:
		KeyInput(const KeyCode keycode)
			: m_KeyCode(keycode) {}

		KeyCode m_KeyCode;

	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }
	};

	class KeyPressedInput : public KeyInput
	{
	private:
		bool m_IsRepeat;

	public:
		KeyPressedInput(const KeyCode keycode, bool isRepeat = false)
			: KeyInput(keycode), m_IsRepeat(isRepeat) {}

		bool IsRepeat() const { return m_IsRepeat; }

		InputType GetInputType() const override { return InputType::KeyPressed; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedInput: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
			return ss.str();
		}
	};

	class KeyReleasedInput : public KeyInput
	{
	public:
		KeyReleasedInput(const KeyCode keycode)
			: KeyInput(keycode) {}

		InputType GetInputType() const override { return InputType::KeyReleased; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedInput: " << m_KeyCode;
			return ss.str();
		}
	};

	class KeyTypedInput : public KeyInput
	{
	public:
		KeyTypedInput(const KeyCode keycode)
			: KeyInput(keycode) {}

		InputType GetInputType() const override { return InputType::KeyTyped; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedInput: " << m_KeyCode;
			return ss.str();
		}
	};


	// MOUSE

	class MouseMovedInput : public Input
	{
	private:
		float m_MouseX, m_MouseY;

	public:
		MouseMovedInput(const float x, const float y)
			: m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		InputType GetInputType() const override { return InputType::MouseMoved; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedInput: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}
	};

	class MouseScrolledInput : public Input
	{
	private:
		float m_XOffset, m_YOffset;

	public:
		MouseScrolledInput(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		InputType GetInputType() const override { return InputType::MouseScrolled; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledInput: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}
	};

	class MouseButtonInput : public Input
	{
	protected:
		MouseButtonInput(const MouseCode button)
			: m_Button(button) {}

		MouseCode m_Button;

	public:
		MouseCode GetMouseButton() const { return m_Button; }
	};

	class MouseButtonPressedInput : public MouseButtonInput
	{
	public:
		MouseButtonPressedInput(const MouseCode button)
			: MouseButtonInput(button) {}

		InputType GetInputType() const override { return InputType::MouseButtonPressed; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedInput: " << m_Button;
			return ss.str();
		}
	};

	class MouseButtonReleasedInput : public MouseButtonInput
	{
	public:
		MouseButtonReleasedInput(const MouseCode button)
			: MouseButtonInput(button) {}

		InputType GetInputType() const override { return InputType::MouseButtonReleased; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedInput: " << m_Button;
			return ss.str();
		}
	};
}