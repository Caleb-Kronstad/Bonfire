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
	void OnInput(Input& input) override;

private:

};