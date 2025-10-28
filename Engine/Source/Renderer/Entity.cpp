#include "bonfire_pch.hpp"
#include "Entity.hpp"

#include "Shader.hpp"
#include "Scene.hpp"
#include "Animation/Animator.hpp"

namespace Bonfire
{

	void Entity::Draw(Camera& camera, std::shared_ptr<Shader> shader, Scene& scene, glm::mat4& manipulation_matrix, glm::mat4& view_matrix, glm::mat4& projection_matrix)
	{
		if (!enabled) return;
		if (!HasComponent<ModelComponent>()) return;
		
		ModelComponent& model_component = GetComponent<ModelComponent>();

		if (!model_component.enabled || !model_component.model || !model_component.shader) return;

		manipulation_matrix = GetWorldTransformMatrix(scene.GetEntities());

		shader->Use();
		shader->SetMat4("projection", projection_matrix);
		shader->SetMat4("view", view_matrix);\
		
		if (shader->name == "Unlit")
		{
		}
		else if (shader->name == "Lit")
		{
			if (!shader->updated_this_frame)
			{
				shader->SetVec3("view_pos", camera.position);
				shader->SetFloat("far_plane", scene.GetShadowMap()->far_plane);
				shader->SetMat4("light_space_matrix", scene.GetShadowMap()->light_space_matrix);
				shader->SetBool("reverse_normals", false);
				scene.UpdateLightSources(*shader);
				scene.GetShadowMap()->Draw();
				scene.GetShadowMap()->updated_this_frame = true;
			}
			
			if (model_component.model->IsAnimated() && HasComponent<AnimationComponent>())
			{
				AnimationComponent& animation_component = GetComponent<AnimationComponent>();
				if (!shader->updated_this_frame)
				{
					const std::vector<glm::mat4>& bone_transforms = animation_component.animator->GetBoneTransforms();
					Log::Info("Uploading " + std::to_string(bone_transforms.size()) + " bone transforms");

					// Check first bone transform
					if (!bone_transforms.empty())
					{
						glm::mat4 first = bone_transforms[0];
						Log::Info("First bone: [" + std::to_string(first[0][0]) + ", " + std::to_string(first[1][1]) + ", " + std::to_string(first[2][2]) + ", " + std::to_string(first[3][3]) + "]");
					}
				}
				if (animation_component.animator)
				{
					const std::vector<glm::mat4>& bone_transforms = animation_component.animator->GetBoneTransforms();
					for (size_t i = 0; i < bone_transforms.size() && i < MAX_BONES; i++)
					{
						shader->SetMat4("bone_transforms["+std::to_string(i)+"]", bone_transforms[i]);
					}
					shader->SetBool("is_animated", true);
				}
				else
					shader->SetBool("is_animated", false);
			}
			else
				shader->SetBool("is_animated", false);
			
			shader->updated_this_frame = true;
		}
		else if (shader->name == "Point Shadow Map")
		{
			if (!shader->updated_this_frame)
			{
				// i dont remember whats supposed to be here
			}
		}
		else if (shader->name == "Shadow Map")
		{
			if (model_component.model->casts_shadow)
				shader->SetMat4("light_space_matrix", scene.GetShadowMap()->light_space_matrix);
		}
		
		shader->SetMat4("model", manipulation_matrix);
		model_component.model->Draw(*shader, model_component.materials);
	}

	void Entity::UpdateComponents(PhysicsSystem& physics_system)
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
                
			physics_body->SetPosition(position);
			physics_body->SetRotation(glm::quat(glm::radians(rotation)));
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
