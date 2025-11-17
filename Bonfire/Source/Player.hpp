#pragma once

#include "BonfireEngine.hpp"

using namespace Bonfire;

class EnemyManager;

struct HitboxCooldown
{
	std::shared_ptr<Entity> hitbox;
	float timer;

	HitboxCooldown(std::shared_ptr<Entity> hitbox, float timer) : hitbox(hitbox), timer(timer) {}
};

struct WeaponStats
{
	std::string name;
	float damage;
	float speed;
	float range;
	glm::vec3 offset;

	WeaponStats(std::string name = "Weapon", glm::vec3 offset = glm::vec3(0.0f), float damage = 1, float speed = 1, float range = 1)
		: name(name), offset(offset), damage(damage), speed(speed), range(range) {}
};

struct PlayerStats
{
	std::string name;
	float max_health;
	float move_speed;
	uint8_t petrification_stacks;

	PlayerStats(std::string name = "Name", float max_health = 10, float move_speed = 5.0f, uint8_t petrification_stacks = 0)
		: name(name), max_health(max_health),  move_speed(move_speed), petrification_stacks(petrification_stacks) { }
};

enum class ItemType
{
	WEAPON,
	CONSUMABLE,
	KEY,
	MISC
};

struct InventoryItem
{
    std::shared_ptr<Texture> icon;
	std::string name;
	std::string description;
	ItemType type = ItemType::MISC;
	int width = 1;
	int height = 1;
	bool rotated = false;

	int grid_x = -1;
	int grid_y = -1;

	int GetWidth() const { return rotated ? height : width; }
	int GetHeight() const { return rotated ? width : height; }
};

struct InventorySlot
{
	bool filled = false;
	std::shared_ptr<InventoryItem> item;
};

class Player : public Layer
{
public:
	Player();
	~Player() override;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const float& delta_time) override;
	void OnInterfaceUpdate() override;
	void OnInput(Input& input) override;

	void TakeDamage(float damage, std::shared_ptr<Entity> hitbox);

	void DrawHealthBar();

private:
	glm::vec3 CalculateCameraBob(const float& delta_time, bool is_moving);
	void CheckWeaponHit();

	bool CanPlaceItem(int grid_x, int grid_y, const InventoryItem& item, int ignore_index = -1);
	bool PlaceItem(int grid_x, int grid_y, InventoryItem item);
	bool RemoveItem(int item_index);
	void RotateSelectedItem();
	int GetItemAt(int grid_x, int grid_y);
	std::pair<int, int> FindFreeSpace(const InventoryItem& item);
	void InitializeInventory();
	void DrawInventoryGrid();

public:
	PlayerStats player_stats;
	WeaponStats weapon_stats;
	float default_move_speed;
	bool camera_can_move;

private:
	float current_health;

	static constexpr int INVENTORY_ROWS = 6;
	static constexpr int INVENTORY_COLUMNS = 10;

	std::array<std::array<int, INVENTORY_COLUMNS>, INVENTORY_ROWS> inventory_grid;
	std::vector<InventoryItem> inventory_items;

	int selected_slot_x = 0;
	int selected_slot_y = 0;
	int dragging_item_index = -1;
	ImVec2 drag_offset = ImVec2(0, 0);
	bool is_dragging = false;
	bool inventory_open = false;
	
	std::vector<HitboxCooldown> recent_hitbox_cooldowns;
	const float DAMAGE_COOLDOWN_TIME = 1.0f;

	glm::vec3 spawn;
	
	std::shared_ptr<Entity> player;
	std::shared_ptr<Entity> arm;
	std::shared_ptr<Entity> weapon;
	
	std::shared_ptr<Audio> music_audio;

	glm::vec3 camera_offset = glm::vec3(0.0f, 2.5f, 0.0f);
	glm::vec3 arm_offset = glm::vec3(1.2f, -1.5f, 1.0f);

	bool is_attacking = false;
	float attack_timer = 0.0f;
	float attack_duration = 1.0f;
	glm::vec3 attack_rotation_start = glm::vec3(0.0f, 0.0f, -90.0f);
	glm::vec3 attack_rotation_end = glm::vec3(0.0f, 0.0f, 90.0f);
	
	bool already_hit_this_swing = false;
	EnemyManager* stone_golem_layer = nullptr;

	float bob_timer = 0.0f;
	float bob_frequency = 10.0f;
	float bob_vertical_amplitude = 0.1f;
	float bob_horizontal_amplitude = 0.0f;
	bool has_smooth_transition = true;
	float bob_transition_speed = 5.0f;
	float current_bob_intensity = 0.0f;
};