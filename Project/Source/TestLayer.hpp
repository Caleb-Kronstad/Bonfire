#ifndef BONFIRE_TEST_LAYER_HPP
#define BONFIRE_TEST_LAYER_HPP

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

};

#endif