#pragma once

#include "BonfireEngine.hpp"

using namespace Bonfire;

class MainLayer : public Layer
{
public:
	MainLayer();
	~MainLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const float& delta_time) override;
	void OnInterfaceUpdate() override;
	void OnInput(Input& input) override;

private:
	std::shared_ptr<Entity> player;
};