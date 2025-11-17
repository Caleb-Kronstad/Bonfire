#include "Player.hpp"
#include <set>

void Player::InitializeInventory()
{
    for (int y = 0; y < INVENTORY_ROWS; y++)
        for (int x = 0; x < INVENTORY_COLUMNS; x++)
            inventory_grid[y][x] = -1;
    inventory_items.clear();
}
bool Player::CanPlaceItem(int grid_x, int grid_y, const InventoryItem& item, int ignore_index)
{
    int w = item.GetWidth();
    int h = item.GetHeight();
    
    if (grid_x < 0 || grid_y < 0 ||
        grid_x + w > INVENTORY_COLUMNS || grid_y + h > INVENTORY_ROWS)
        return false;
        
    for (int dy = 0; dy < h; dy++)
    {
        for (int dx = 0; dx < w; dx++)
        {
            int cell = inventory_grid[grid_y + dy][grid_x + dx];
            if (cell != -1 && cell != ignore_index)
                return false;
        }
    }
    return true;
}

bool Player::PlaceItem(int grid_x, int grid_y, InventoryItem item)
{
    if (!CanPlaceItem(grid_x, grid_y, item))
        return false;

    item.grid_x = grid_x;
    item.grid_y = grid_y;

    int item_index = static_cast<int>(inventory_items.size());
    inventory_items.push_back(item);

    int w = item.GetWidth();
    int h = item.GetHeight();
    for (int dy = 0; dy < h; dy++)
        for (int dx = 0; dx < w; dx++)
            inventory_grid[grid_y + dy][grid_x + dx] = item_index;

    return true;
}

bool Player::RemoveItem(int item_index)
{
    if (item_index < 0 || item_index >= inventory_items.size())
        return false;
        
    for (int y = 0; y < INVENTORY_ROWS; y++)
    {
        for (int x = 0; x < INVENTORY_COLUMNS; x++)
        {
            if (inventory_grid[y][x] == item_index)
                inventory_grid[y][x] = -1;
                
            else if (inventory_grid[y][x] > item_index)
                inventory_grid[y][x]--;
        }
    }
    inventory_items.erase(inventory_items.begin() + item_index);
    return true;
}

int Player::GetItemAt(int grid_x, int grid_y)
{
    if (grid_x < 0 || grid_x >= INVENTORY_COLUMNS || grid_y < 0 || grid_y >= INVENTORY_ROWS)
        return -1;
    return inventory_grid[grid_y][grid_x];
}
void Player::RotateSelectedItem()
{
    int item_index = GetItemAt(selected_slot_x, selected_slot_y);
    if (item_index == -1) return;
    InventoryItem& item = inventory_items[item_index];
    int item_x = selected_slot_x, item_y = selected_slot_y;
    while (item_x > 0 && inventory_grid[item_y][item_x - 1] == item_index) item_x--;
    while (item_y > 0 && inventory_grid[item_y - 1][item_x] == item_index) item_y--;
    for (int y = 0; y < INVENTORY_ROWS; y++)
        for (int x = 0; x < INVENTORY_COLUMNS; x++)
            if (inventory_grid[y][x] == item_index)
                inventory_grid[y][x] = -1;
    item.rotated = !item.rotated;
    if (!CanPlaceItem(item_x, item_y, item, item_index))
        item.rotated = !item.rotated;
    int w = item.GetWidth();
    int h = item.GetHeight();
    for (int dy = 0; dy < h; dy++)
        for (int dx = 0; dx < w; dx++)
            inventory_grid[item_y + dy][item_x + dx] = item_index;
}

std::pair<int, int> Player::FindFreeSpace(const InventoryItem& item)
{
    for (int y = 0; y < INVENTORY_ROWS; y++)
    {
        for (int x = 0; x < INVENTORY_COLUMNS; x++)
        {
            if (CanPlaceItem(x, y, item))
                return {x, y};
        }
    }
    return {-1, -1};
}

void Player::DrawInventoryGrid()
{
    if (!inventory_open) return;
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    int padding = 200;
    ImVec2 window_size(INVENTORY_COLUMNS*50, INVENTORY_ROWS*50 + padding);
    ImVec2 screen_size = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2((screen_size.x - window_size.x) * 0.5f, (screen_size.y - window_size.y) * 0.5f));
    ImGui::Begin("Inventory", &inventory_open, window_flags);
    
    const float cell_size = 50.0f;
    ImVec2 grid_origin = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 mouse_pos = ImGui::GetMousePos();
    
    int hover_x = static_cast<int>((mouse_pos.x - grid_origin.x) / cell_size);
    int hover_y = static_cast<int>((mouse_pos.y - grid_origin.y) / cell_size);
    bool valid_hover = hover_x >= 0 && hover_x < INVENTORY_COLUMNS &&
                       hover_y >= 0 && hover_y < INVENTORY_ROWS;
    
    for (int y = 0; y < INVENTORY_ROWS; y++)
    {
        for (int x = 0; x < INVENTORY_COLUMNS; x++)
        {
            ImVec2 p_min(grid_origin.x + x * cell_size, grid_origin.y + y * cell_size);
            ImVec2 p_max(p_min.x + cell_size, p_min.y + cell_size);
    
            ImU32 bg_color = IM_COL32(40, 40, 40, 255);
            draw_list->AddRectFilled(p_min, p_max, bg_color);
            draw_list->AddRect(p_min, p_max, IM_COL32(80, 80, 80, 255));
    
            if (x == selected_slot_x && y == selected_slot_y)
            {
                draw_list->AddRect(p_min, p_max, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);
            }
        }
    }
    
    if (is_dragging && dragging_item_index != -1 && valid_hover)
    {
        const InventoryItem& drag_item = inventory_items[dragging_item_index];
        bool can_place = CanPlaceItem(hover_x, hover_y, drag_item, dragging_item_index);
    
        ImVec2 preview_min(grid_origin.x + hover_x * cell_size,
                           grid_origin.y + hover_y * cell_size);
        ImVec2 preview_max(preview_min.x + drag_item.GetWidth() * cell_size,
                           preview_min.y + drag_item.GetHeight() * cell_size);
    
        ImU32 preview_color = can_place
            ? IM_COL32(0, 255, 0, 100) 
            : IM_COL32(255, 0, 0, 100); 
    
        draw_list->AddRectFilled(preview_min, preview_max, preview_color);
    }
    
    std::set<int> drawn_items;
    for (int y = 0; y < INVENTORY_ROWS; y++)
    {
        for (int x = 0; x < INVENTORY_COLUMNS; x++)
        {
            int item_idx = inventory_grid[y][x];
            if (item_idx != -1 && !drawn_items.contains(item_idx))
            {
                drawn_items.insert(item_idx);
    
                if (is_dragging && item_idx == dragging_item_index)
                    continue;
    
                const InventoryItem& item = inventory_items[item_idx];
                ImVec2 p_min(grid_origin.x + x * cell_size, grid_origin.y + y * cell_size);
                ImVec2 p_max(p_min.x + item.GetWidth() * cell_size,
                             p_min.y + item.GetHeight() * cell_size);
    
                draw_list->AddRect(p_min, p_max, IM_COL32(200, 200, 200, 255), 0.0f, 0, 2.0f);
    
                if (item.icon)
                {
                    ImTextureID tex_id = (ImTextureID)(intptr_t)item.icon->gl_id;
    
                    if (item.rotated)
                    {
                        ImVec2 uv0(0, 1), uv1(0, 0), uv2(1, 0), uv3(1, 1);
                        draw_list->AddImageQuad(tex_id, p_min,
                            ImVec2(p_max.x, p_min.y), p_max, ImVec2(p_min.x, p_max.y),
                            uv0, uv1, uv2, uv3);
                    }
                    else
                    {
                        draw_list->AddImage(tex_id, p_min, p_max);
                    }
                }
                else
                {
                    ImU32 item_color;
                    switch (item.type)
                    {
                        case ItemType::WEAPON:  item_color = IM_COL32(150, 50, 50, 200); break;
                        case ItemType::CONSUMABLE: item_color = IM_COL32(50, 150, 50, 200); break;
                        case ItemType::KEY:    item_color = IM_COL32(150, 150, 50, 200); break;
                        case ItemType::MISC: item_color = IM_COL32(50, 50, 150, 200); break;
                        default:                item_color = IM_COL32(100, 100, 100, 200); break;
                    }
                    draw_list->AddRectFilled(p_min, p_max, item_color);
  
                    ImVec2 text_size = ImGui::CalcTextSize(item.name.c_str());
                    ImVec2 text_pos(
                        p_min.x + (item.GetWidth() * cell_size - text_size.x) * 0.5f,
                        p_min.y + (item.GetHeight() * cell_size - text_size.y) * 0.5f
                    );
                    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), item.name.c_str());
                }
            }
        }
    }
  
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && valid_hover && !is_dragging)
        {
            int clicked_item = GetItemAt(hover_x, hover_y);
            if (clicked_item != -1)
            {
                dragging_item_index = clicked_item;
                is_dragging = true;
  
                InventoryItem& item = inventory_items[clicked_item];
                drag_offset = ImVec2(
                    mouse_pos.x - (grid_origin.x + item.grid_x * cell_size),
                    mouse_pos.y - (grid_origin.y + item.grid_y * cell_size)
                );
            }
  
            selected_slot_x = hover_x;
            selected_slot_y = hover_y;
        }
  
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && valid_hover)
        {
            selected_slot_x = hover_x;
            selected_slot_y = hover_y;
            RotateSelectedItem();
        }
    }
  
    if (is_dragging && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        if (valid_hover && dragging_item_index != -1)
        {
            InventoryItem& item = inventory_items[dragging_item_index];
  
            if (CanPlaceItem(hover_x, hover_y, item, dragging_item_index))
            {
                for (int y = 0; y < INVENTORY_ROWS; y++)
                    for (int x = 0; x < INVENTORY_COLUMNS; x++)
                        if (inventory_grid[y][x] == dragging_item_index)
                            inventory_grid[y][x] = -1;
  
                int w = item.GetWidth();
                int h = item.GetHeight();
                for (int dy = 0; dy < h; dy++)
                    for (int dx = 0; dx < w; dx++)
                        inventory_grid[hover_y + dy][hover_x + dx] = dragging_item_index;
  
                item.grid_x = hover_x;
                item.grid_y = hover_y;
            }
        }
  
        is_dragging = false;
        dragging_item_index = -1;
    }
  
    if (is_dragging && dragging_item_index != -1)
    {
        const InventoryItem& item = inventory_items[dragging_item_index];
        ImVec2 drag_min(mouse_pos.x - drag_offset.x, mouse_pos.y - drag_offset.y);
        ImVec2 drag_max(drag_min.x + item.GetWidth() * cell_size,
                        drag_min.y + item.GetHeight() * cell_size);
  
        if (item.icon)
        {
            ImTextureID tex_id = (ImTextureID)(intptr_t)item.icon->gl_id;

            if (item.rotated)
            {
                ImVec2 uv_tl(0.0f, 1.0f);
                ImVec2 uv_tr(0.0f, 0.0f);
                ImVec2 uv_br(1.0f, 0.0f);
                ImVec2 uv_bl(1.0f, 1.0f);

                draw_list->AddImageQuad(
                    tex_id,
                    drag_min,
                    ImVec2(drag_max.x, drag_min.y),
                    drag_max,
                    ImVec2(drag_min.x, drag_max.y),
                    uv_tl, uv_tr, uv_br, uv_bl,
                    IM_COL32(255, 255, 255, 180)
                );
            }
            else
            {
                draw_list->AddImage(tex_id, drag_min, drag_max,
                                   ImVec2(0,0), ImVec2(1,1), IM_COL32(255, 255, 255, 180));
            }
        }
        else
        {
            draw_list->AddRectFilled(drag_min, drag_max, IM_COL32(150, 150, 150, 180));
            ImVec2 text_size = ImGui::CalcTextSize(item.name.c_str());
            ImVec2 text_pos(
                drag_min.x + (item.GetWidth() * cell_size - text_size.x) * 0.5f,
                drag_min.y + (item.GetHeight() * cell_size - text_size.y) * 0.5f
            );
            draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), item.name.c_str());
        }
  
        draw_list->AddRect(drag_min, drag_max, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);
    }
  
    ImGui::Dummy(ImVec2(INVENTORY_COLUMNS * cell_size, INVENTORY_ROWS * cell_size));
    ImGui::Separator();
  
    int selected_item = GetItemAt(selected_slot_x, selected_slot_y);
    if (selected_item != -1)
    {
        const InventoryItem& item = inventory_items[selected_item];
  
        if (item.icon)
        {
            ImTextureID tex_id = (ImTextureID)(intptr_t)item.icon->gl_id;
            ImGui::Image(tex_id, ImVec2(64, 64));
            ImGui::SameLine();
        }
  
        ImGui::BeginGroup();
        ImGui::Text("%s", item.name.c_str());
        ImGui::TextWrapped("%s", item.description.c_str());
        ImGui::Text("Size: %dx%d", item.width, item.height);
        ImGui::EndGroup();
  
        ImGui::Spacing();
        if (ImGui::Button("Rotate (R)"))
            RotateSelectedItem();
        ImGui::SameLine();
        if (ImGui::Button("Drop"))
            RemoveItem(selected_item);
    }
    else
    {
        ImGui::Text("No item selected");
    }
  
    ImGui::End();
}