#include "bonfire_pch.hpp"
#include "Entity.hpp"

#include "Shader.hpp"
#include "Scene.hpp"
#include "Animation/Animator.hpp"
#include "Core/Project.hpp"

namespace Bonfire
{

	void Entity::Draw(std::shared_ptr<Shader> shader, Scene& scene)
	{
		if (!enabled) return;
		if (!HasComponent<ModelComponent>()) return;
		
		ModelComponent& model_component = GetComponent<ModelComponent>();

		if (!model_component.enabled || !model_component.model || !shader) return;

		glm::mat4 manipulation_matrix = GetWorldTransformMatrix(scene.GetEntities());
		if (model_component.model->name == "StoneGolem")
		{
			manipulation_matrix = glm::rotate(manipulation_matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		
		shader->SetMat4("model", manipulation_matrix);
		model_component.model->Draw(*shader, model_component.material);
	}

	void Entity::UpdateComponents()
	{
		if (HasComponent<LightSourceComponent>())
		{
			LightSourceComponent& light_source_component = GetComponent<LightSourceComponent>();
			if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
			{
				point_light->position = position;
				point_light->scale = scale;
			}
			else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_source_component.light_source))
			{
				spot_light->position = position;
				spot_light->scale = scale;
			}
		}
		if (HasComponent<PhysicsComponent>())
		{
			PhysicsComponent& physics_component = GetComponent<PhysicsComponent>();
			std::shared_ptr<PhysicsBody> physics_body = physics_component.physics_body;

			if (Project::GetInstance().GetProjectRunState())
			{
				position = physics_body->GetPosition();
				rotation = glm::degrees(glm::eulerAngles(physics_body->GetRotation()));
			}
			else
			{
				physics_body->SetPosition(position);
				physics_body->SetRotation(glm::quat(glm::radians(rotation)));
			}
		}
		if (HasComponent<AudioComponent>())
		{
			AudioComponent& audio_component = GetComponent<AudioComponent>();
			if (audio_component.audio && audio_component.enabled)
			{
				audio_component.audio->Set3DPosition(position);
				if (audio_component.audio->GetPlayOnAwake() && !audio_component.audio->IsPlaying())
					audio_component.audio->Play();
			}
		}
	}

	void Entity::ValidateDOFS()
	{
		if (HasComponent<PhysicsComponent>())
		{
			PhysicsComponent& physics_component = GetComponent<PhysicsComponent>();
			PhysicsBody& physics_body = *physics_component.physics_body;
			physics_body.SetAllowedDOFS(
				physics_component.can_move_axis[0], physics_component.can_move_axis[1], physics_component.can_move_axis[2],
				physics_component.can_rotate_axis[0], physics_component.can_rotate_axis[1], physics_component.can_rotate_axis[2]);
		}
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
	
	bool Entity::AddComponent(ComponentType type, std::shared_ptr<Component> component)
	{
		if (components.contains(type))
		{
			Log::Warning("Component " + std::to_string(component->id) +" already added to Entity " + std::to_string(id));
			return false;
		}
		components.insert_or_assign(type, component);
		return true;
	}
	bool Entity::RemoveComponent(ComponentType type)
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
	
	glm::quat Entity::GetTransformOrientation() const
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

	glm::vec3 Entity::GetForwardVector() const
	{
		return glm::normalize(GetTransformOrientation() * glm::vec3(0.0f, 0.0f, 1.0f));
	}
	glm::vec3 Entity::GetRightVector() const
	{
		return glm::normalize(GetTransformOrientation() * glm::vec3(-1.0f, 0.0f, 0.0f));
	}
	glm::vec3 Entity::GetUpVector() const
	{
		return glm::normalize(GetTransformOrientation() * glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
