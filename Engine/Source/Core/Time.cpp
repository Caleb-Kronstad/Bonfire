#include "bonfire_pch.h"
#include "Time.h"

namespace Bonfire
{
	void Time::TickDeltaTime()
	{
		float currentFrameTime = static_cast<float>(glfwGetTime());
		m_DeltaTime = currentFrameTime - m_LastFrameTime;
		m_LastFrameTime = currentFrameTime;
	}
}