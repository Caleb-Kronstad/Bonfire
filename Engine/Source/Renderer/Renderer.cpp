#include "bonfire_pch.h"
#include "Renderer.h"

#include "Input/Input.h"
#include "Input/InputCodes.h"
#include "Input/InputTypes.h"

namespace Bonfire
{
	Renderer::Renderer()
	{

	}
	Renderer::~Renderer()
	{

	}

	void Renderer::OnAttach()
	{
	}
	void Renderer::OnDetach()
	{
	}

	void Renderer::OnUpdate()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::OnInput(Input& input)
	{
	}
}