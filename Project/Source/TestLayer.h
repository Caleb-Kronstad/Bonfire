#pragma once

#include "Engine.h"

using namespace Bonfire;

class TestLayer : public Layer
{
public:
	TestLayer();
	~TestLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnInput(Bonfire::Input& input) override;

private:

};