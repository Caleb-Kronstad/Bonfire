#include "bonfire_pch.hpp"
#include "Entity.hpp"

#include "Shader.hpp"
#include "Scene.hpp"

namespace Bonfire
{

	void Entity::Draw(std::shared_ptr<Shader> shader, Scene& scene, glm::mat4& manipulation_matrix, glm::mat4& view_matrix, glm::mat4& projection_matrix)
	{
		if (!enabled) return;
		if (!HasComponent<ModelComponent>()) return;
		
		ModelComponent& model_component = GetComponent<ModelComponent>();

		if (!model_component.enabled || !model_component.model || !model_component.material) return;

		manipulation_matrix = GetWorldTransformMatrix(scene.GetEntities());

		shader->Use();
		shader->SetMat4("projection", projection_matrix);
		shader->SetMat4("view", view_matrix);
		
		if (shader->name == "Unlit")
		{
		}
		if (shader->name == "Lit")
		{
			if (!shader->updated_this_frame && !scene.GetShadowMap()->updated_this_frame)
			{
				shader->SetVec3("view_pos", scene.GetEngineCamera()->Position);
				shader->SetFloat("far_plane", scene.GetShadowMap()->far_plane);
				shader->SetMat4("lightSpaceMatrix", glm::mat4(1.0f));
				shader->SetBool("reverseNormals", false);
				scene.UpdateLightSources(*shader);
				scene.GetShadowMap()->Draw();
				scene.GetShadowMap()->updated_this_frame = true;
				shader->updated_this_frame = true;
			}
		}
		if (shader->name == "Point Shadow Map")
		{
			if (!shader->updated_this_frame)
			{
				
			}
		}
		
		shader->SetMat4("model", manipulation_matrix);
		model_component.model->Draw(*shader, model_component.material);
	}


	AABB Entity::GetWorldAABB(const std::unordered_map<uint32_t, std::shared_ptr<Entity>>& entities)
	{
		AABB world_aabb;
		if (!HasComponent<ModelComponent>()) return world_aabb;
		ModelComponent& model_component = GetComponent<ModelComponent>();
		if (!model_component.model || !model_component.enabled) return world_aabb;

		AABB local_aabb = model_component.model->CalculateAABB();
		glm::mat4 world_transform = GetWorldTransformMatrix(entities);
		world_aabb = TransformAABB(local_aabb, world_transform);

		return world_aabb;
	}
	
	bool Entity::AddComponent(COMPONENT_TYPE type, std::shared_ptr<Component> component)
	{
		if (components.contains(type))
		{
			Log::Warning("Component " + std::to_string(component->id) +" already added to Entity " + std::to_string(id));
			return false;
		}
		components.insert_or_assign(type, component);
		return true;
	}
	bool Entity::RemoveComponent(COMPONENT_TYPE type)
	{
		if (components.contains(type))
		{
			components.erase(type);
			return true;
		}
		Log::Warning("Component not found on Entity " + std::to_string(id));
		return false;
	}
	bool Entity::AddChild(uint32_t child_id)
	{
		auto it = std::find(children.begin(), children.end(), child_id);
		if (it != children.end())
		{
			Log::Warning("Entity " + std::to_string(child_id) + " already child of Entity " + std::to_string(id));
			return false;
		}
		children.push_back(child_id);
		return true;
	}
	bool Entity::RemoveChild(uint32_t child_id)
	{
		auto it = std::find(children.begin(), children.end(), child_id);
		if (it != children.end())
		{
			children.erase(it);
			return true;
		}
		Log::Warning("Child " + std::to_string(child_id) + " not found in Entity " + std::to_string(id));
		return false;
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
	glm::mat4 Entity::GetWorldTransformMatrix(const std::unordered_map<uint32_t, std::shared_ptr<Entity>>& entities)
	{
		glm::mat4 local_transform = GetTransformMatrix();

		if (parent == 0 || !entities.contains(parent))
			return local_transform;

		glm::mat4 parent_world_transform = entities.at(parent)->GetWorldTransformMatrix(entities);
		return parent_world_transform * local_transform;
	}
}
