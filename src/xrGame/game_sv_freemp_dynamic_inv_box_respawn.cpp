#include "StdAfx.h"
#include "game_sv_freemp.h"
#include "alife_simulator.h"
#include "Level.h"

void game_sv_freemp::DynamicBoxUpdate()
{
	if (Level().game && BoxResawnTimer <= Device.dwTimeGlobal)
	{
		BoxResawnTimer = Device.dwTimeGlobal + (box_respawn_time * 1000);
		for (const auto &entity : inventory_boxes_cse)
		{
			CSE_Abstract* abs = entity.second.entity;
			CSE_ALifeInventoryBox* box = smart_cast<CSE_ALifeInventoryBox*>(abs);;

			if (box->children.empty())
			{
				SpawnInvBoxesItems(box);
			}
		}
	}
}

void game_sv_freemp::DynamicBoxFileCreate()
{
	FS.update_path(m_inventory_box_config_path, "$game_config$", "alife\\inventory_boxes.json");

	std::ifstream file(m_inventory_box_config_path);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();

		if (!m_inventory_box_config_json.parse(buffer.str()))
		{
			Msg("! ERROR: Failed to parse inventory_boxes.json");
		}
	}
	else
	{
		Msg("! ERROR: Failed to open inventory_boxes.json at path: %s", m_inventory_box_config_path);
	}
}


void game_sv_freemp::SpawnInvBoxesItems(CSE_ALifeInventoryBox* box)
{
	LPCSTR boxfile = box->m_ini_string.c_str();

	jsonxx::Object* spawn_section_obj = nullptr;

	if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_trash") && xr_strcmp(box->m_ini_string.c_str(), "spawn_trash") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_trash");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_boosters") && xr_strcmp(box->m_ini_string.c_str(), "spawn_boosters") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_boosters");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_weapons_devices") && xr_strcmp(box->m_ini_string.c_str(), "spawn_weapons_devices") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_weapons_devices");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_ammo") && xr_strcmp(box->m_ini_string.c_str(), "spawn_ammo") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_ammo");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_explosive") && xr_strcmp(box->m_ini_string.c_str(), "spawn_explosive") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_explosive");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_weapons") && xr_strcmp(box->m_ini_string.c_str(), "spawn_weapons") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_weapons");
	}

	if (spawn_section_obj != nullptr && spawn_section_obj->has<jsonxx::Array>("spawn"))
	{
		jsonxx::Array& spawn_items_array = spawn_section_obj->get<jsonxx::Array>("spawn");
		for (size_t k = 0; k < spawn_items_array.size(); ++k)
		{
			if (!spawn_items_array.has<jsonxx::Object>((unsigned int)k)) continue;
			jsonxx::Object& item_obj = spawn_items_array.get<jsonxx::Object>((unsigned int)k);

			std::string N = item_obj.get<jsonxx::String>("name", std::string());
			int j = (int)item_obj.get<jsonxx::Number>("count", 0);
			float p = (float)item_obj.get<jsonxx::Number>("probability", 0);
			float f_cond = (float)item_obj.get<jsonxx::Number>("condition", 0);
			bool bScope = item_obj.get<jsonxx::Boolean>("scope", false);
			bool bSilencer = item_obj.get<jsonxx::Boolean>("silencer", false);
			bool bLauncher = item_obj.get<jsonxx::Boolean>("launcher", false);
			int cur_scope = (int)item_obj.get<jsonxx::Number>("cur_scope", 0);

			VERIFY(N.length());

			for (u32 i = 0; i < j; ++i) {
				if (Random.randF(1.f) < p) {
					CSE_Abstract* E = spawn_begin(N.c_str());
					E->ID_Parent = box->ID;

					CSE_ALifeItemWeapon* W = smart_cast<CSE_ALifeItemWeapon*>(E);
					if (W) {
						if (W->m_scope_status == ALife::eAddonAttachable)
						{
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
							W->m_cur_scope = cur_scope;
						}
						if (W->m_silencer_status == ALife::eAddonAttachable)
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
						if (W->m_grenade_launcher_status == ALife::eAddonAttachable)
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);
					}
					CSE_ALifeInventoryItem* IItem = smart_cast<CSE_ALifeInventoryItem*>(E);
					if (IItem)
					{
						f_cond = Random.randF(0.0f, 0.6f);
						IItem->m_fCondition = f_cond;
					}
					spawn_end(E, m_server->GetServerClient()->ID);
				}
			}
		}
	}
}

void game_sv_freemp::OnStartSpawnInvBoxesItems(CSE_ALifeInventoryBox* box)
{
	LPCSTR boxfile = box->m_ini_string.c_str();

	jsonxx::Object* spawn_section_obj = nullptr;

	if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_trash") && xr_strcmp(box->m_ini_string.c_str(), "spawn_trash") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_trash");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_boosters") && xr_strcmp(box->m_ini_string.c_str(), "spawn_boosters") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_boosters");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_weapons_devices") && xr_strcmp(box->m_ini_string.c_str(), "spawn_weapons_devices") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_weapons_devices");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_ammo") && xr_strcmp(box->m_ini_string.c_str(), "spawn_ammo") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_ammo");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_explosive") && xr_strcmp(box->m_ini_string.c_str(), "spawn_explosive") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_explosive");
	}
	else if (m_inventory_box_config_json.has<jsonxx::Object>("spawn_weapons") && xr_strcmp(box->m_ini_string.c_str(), "spawn_weapons") == 0)
	{
		spawn_section_obj = &m_inventory_box_config_json.get<jsonxx::Object>("spawn_weapons");
	}

	if (spawn_section_obj != nullptr && spawn_section_obj->has<jsonxx::Array>("spawn"))
	{
		jsonxx::Array& spawn_items_array = spawn_section_obj->get<jsonxx::Array>("spawn");
		for (size_t k = 0; k < spawn_items_array.size(); ++k)
		{
			if (!spawn_items_array.has<jsonxx::Object>((unsigned int)k)) continue;
			jsonxx::Object& item_obj = spawn_items_array.get<jsonxx::Object>((unsigned int)k);

			std::string N = item_obj.get<jsonxx::String>("name", std::string());
			int j = (int)item_obj.get<jsonxx::Number>("count", 0);
			float p = (float)item_obj.get<jsonxx::Number>("probability", 0);
			float f_cond = (float)item_obj.get<jsonxx::Number>("condition", 0);
			bool bScope = item_obj.get<jsonxx::Boolean>("scope", false);
			bool bSilencer = item_obj.get<jsonxx::Boolean>("silencer", false);
			bool bLauncher = item_obj.get<jsonxx::Boolean>("launcher", false);
			int cur_scope = (int)item_obj.get<jsonxx::Number>("cur_scope", 0);

			VERIFY(N.length());

			for (u32 i = 0; i < j; ++i) {
				if (Random.randF(1.f) < p) {
					CSE_Abstract* E = alife().spawn_item(N.c_str(), box->o_Position, box->m_tNodeID, box->m_tGraphID, box->ID);

					CSE_ALifeItemWeapon* W = smart_cast<CSE_ALifeItemWeapon*>(E);
					if (W) {
						if (W->m_scope_status == ALife::eAddonAttachable)
						{
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
							W->m_cur_scope = cur_scope;
						}
						if (W->m_silencer_status == ALife::eAddonAttachable)
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
						if (W->m_grenade_launcher_status == ALife::eAddonAttachable)
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);
					}
					CSE_ALifeInventoryItem* IItem = smart_cast<CSE_ALifeInventoryItem*>(E);
					if (IItem)
					{
						f_cond = Random.randF(0.0f, 0.6f);
						IItem->m_fCondition = f_cond;
					}
				}
			}
		}
	}
}
