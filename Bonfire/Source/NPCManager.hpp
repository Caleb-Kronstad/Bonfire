#pragma once

#include "BonfireEngine.hpp"
#include "Player.hpp"

using namespace Bonfire;

struct NPC
{
    std::shared_ptr<Entity> entity;
    float interact_range = 5.0f;
    bool can_interact = false;
    bool interacting = false;
    int interaction_count = 0;
};

struct PetrifiedNPC
{
    std::shared_ptr<Entity> entity;
    std::string animation;

    PetrifiedNPC(std::shared_ptr<Entity> entity, std::string animation) : entity(entity), animation(animation) {}
};

struct ShopItem : InventoryItem
{
    std::shared_ptr<Texture> icon;
    std::string name;
    std::string description;
    int price;

    ShopItem(std::string name, std::string description, int price) : name(name), description(description), price(price) {}
};

namespace Bonfire
{
    class NPCManager : public Layer
    {
    public:
        NPCManager();
        ~NPCManager() override;
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(const float& delta_time) override;
        void OnInput(Input& input) override;
        void OnInterfaceUpdate() override;

    private:
        void DisplayMerchantShop();

    private:
        std::shared_ptr<Entity> player;
        Player* player_layer = nullptr;
        
        NPC knight;
        NPC merchant;
        NPC forsaken;

        std::vector<ShopItem> shop_items;
        bool displaying_shop = false;

        std::vector<PetrifiedNPC> petrified_npcs;
    };
}
