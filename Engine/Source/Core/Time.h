#pragma once

namespace Bonfire
{
	class Time
	{
	public:
		static float GetDeltaTime() { return m_DeltaTime; }
		static void TickDeltaTime();

	private:
		static float m_DeltaTime;
		static float m_LastFrameTime;
	};
}