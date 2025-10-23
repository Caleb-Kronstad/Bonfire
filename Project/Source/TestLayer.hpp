#pragma once

#include "BonfireEngine.hpp"

using namespace Bonfire;

class TestLayer : public Layer
{
public:
	TestLayer();
	~TestLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnInterfaceUpdate() override;
	void OnInput(Input& input) override;

private:
	std::shared_ptr<Entity> player;
	bool w_down = false, s_down = false, a_down = false, d_down = false;
	bool space_down = false;
	float max_jump_height = 25.0f;

};