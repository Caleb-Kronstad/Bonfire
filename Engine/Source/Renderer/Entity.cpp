#include "bonfire_pch.hpp"
#include "Entity.hpp"

#include "Shader.hpp"

namespace Bonfire
{

	void Entity::Draw(Shader& shader, glm::mat4& manipulation_matrix)
	{
		if (!enabled) return;
		if (!HasComponent<ModelComponent>()) return;
		if (!HasComponent<TextureComponent>()) return;
		
		ModelComponent& model_component = GetComponent<ModelComponent>();
		TextureComponent& texture_component = GetComponent<TextureComponent>();

		if (!model_component.enabled) return;

		manipulation_matrix = GetTransformMatrix();
		shader.SetMat4("model", manipulation_matrix);
		model_component.model->Draw(shader, texture_component.textures);
	}
	
	void Entity::AddComponent(COMPONENT_TYPE type, std::shared_ptr<Component> component)
	{
		components.insert_or_assign(type, component);
	}
	void Entity::RemoveComponent(COMPONENT_TYPE type)
	{
		components.erase(type);
	}
	
	glm::quat Entity::GetTransformOrientation()
	{
		return glm::quat(rotation / 180.0f * glm::pi<float>());
	}
	glm::mat4 Entity::GetTransformMatrix()
	{
		return glm::translate(glm::mat4(1.0f), position)
		* glm::toMat4(GetTransformOrientation())
		* glm::scale(glm::mat4(1.0f), scale);
	}
}
