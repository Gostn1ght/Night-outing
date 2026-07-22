#include "StdAfx.h"
#include "game_sv_freemp.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "Level.h"

// Functions
u32 SendedBytes = 0;
u32 SyncAlifeObjects = 0;
int SyncAlifeCount = 0;


// Updater
void game_sv_freemp::UpdateAlifeData()
{
	if (!map_alife_sended.empty())
	for (auto cl : map_alife_sended)
	{
		WriteAlifeObjectsToClient(cl.first);
		map_alife_sended.erase(cl.first);
	}
 
	UpdateAlifeObjects();
	UpdateAlifeObjectsPOS();

	SyncAlifeObjects = objects_active.size();
}

void game_sv_freemp::WriteAlifeObjectsToClient(ClientID client_id)
{
	if (!ai().get_alife())
		return;
	
  	u16 id = 0;
	
	for (auto& object : objects_active)
	{
		id += 1;

		NET_Packet packet;

		packet.w_begin(M_GAMEMESSAGE);
		packet.w_u32(GAME_EVENT_OBJECTS_SPAWN);

  		packet.w_u8(id == objects_active.size() ? 1 : 0);
  		packet.w_stringZ(object.alife_object->s_name.c_str());

  		object.alife_object->Spawn_WriteNoBeginPacket(packet, false);
 		object.alife_object->UPDATE_Write(packet);
 	
		SendedBytes += packet.B.count;
		
		server().SendTo(client_id, packet, net_flags(true));
	}
}

void game_sv_freemp::UpdateAlifeObjects()
{
	if (!ai().get_alife())
		return;

 	if (last_alife_update_time < Device.dwTimeGlobal)
	{
		last_alife_update_time = Device.dwTimeGlobal + 500;

		std::sort(objects_active.begin(), objects_active.end(), [&](AlifeData& data, AlifeData& data2)
			{
				return data.dwTime < data2.dwTime;
			});

		u32 CSended = 0;
		for (auto& object : objects_active)
		{
			if (
				!smart_cast<CSE_ALifeOnlineOfflineGroup*> (object.alife_object) && !smart_cast<CSE_ALifeSmartZone*> (object.alife_object)
				)
			continue;

			if (CSended > SyncAlifeCount)
				return;
			CSended++;

			NET_Packet packet;
			packet.w_begin(M_GAMEMESSAGE);
			packet.w_u32(GAME_EVENT_OBJECTS_UPDATE);
			packet.w_u8(3);
			packet.w_u16(object.ID);
			packet.w_stringZ(object.alife_object->s_name);
			object.alife_object->UPDATE_Write(packet);
		
			SendedBytes += packet.B.count;
  			server().SendBroadcast(server().GetServerClient()->ID, packet, net_flags(false));

			object.dwTime = Device.dwTimeGlobal;
		}
	}
}

void game_sv_freemp::UpdateAlifeObjectsPOS()
{
	if (!ai().get_alife())
		return;

	if (last_alife_update_time_pos < Device.dwTimeGlobal)
	{
		last_alife_update_time_pos = Device.dwTimeGlobal + 500;
 
		std::sort(objects_active.begin(), objects_active.end(), [&](AlifeData& data, AlifeData& data2)
		{
			return data.dwTime < data2.dwTime;
		});

		u32 CSended = 0;

		for (auto& object : objects_active)
		{
			if (
				!smart_cast<CSE_ALifeHumanStalker*>(object.alife_object) && !smart_cast<CSE_ALifeMonsterAbstract*>(object.alife_object) &&
				!smart_cast<CSE_ALifeCreatureActor*>(object.alife_object) && !smart_cast<CSE_ALifeOnlineOfflineGroup*> (object.alife_object) &&
				!smart_cast<CSE_ALifeSmartZone*> (object.alife_object)
				)
				continue;

			if (CSended > SyncAlifeCount)
				return;
			CSended++;
			 
			NET_Packet packet;
			packet.w_begin(M_GAMEMESSAGE);
			packet.w_u32(GAME_EVENT_OBJECTS_UPDATE);
			packet.w_u8(4);
		
			packet.w_u16(object.alife_object->ID);
			packet.w_vec3(object.alife_object->position());
			packet.w_u16(object.alife_object->m_tGraphID);
			packet.w_u16(object.alife_object->m_tNodeID);

			SendedBytes += packet.B.count;
			server().SendBroadcast(server().GetServerClient()->ID, packet, net_flags(false));

			object.dwTime = Device.dwTimeGlobal;
		}
 
	}
}

void game_sv_freemp::RegisterUpdateAlife(CSE_ALifeDynamicObject* object, bool reg)
{
	if (!ai().get_alife())
		return;
	 
	if (reg)
	{
		AlifeData data;
		data.alife_object = object;
		data.dwTime = Device.dwTimeGlobal;
		data.ID = object->ID;
		objects_active.push_back(data);

		if (Phase() != GAME_PHASE_INPROGRESS)
			return;

		NET_Packet packet;
		packet.w_begin(M_GAMEMESSAGE);
		packet.w_u32(GAME_EVENT_OBJECTS_UPDATE);
		packet.w_u8(1);
		packet.w_u16(object->ID);
		packet.w_stringZ(object->s_name.c_str());
		object->Spawn_WriteNoBeginPacket(packet, false);
		object->UPDATE_Write(packet);
		SendedBytes += packet.B.count;
		server().SendBroadcast(server().GetServerClient()->ID, packet, net_flags(true));
	}
	else
	{
		auto IT = std::find_if(objects_active.begin(), objects_active.end(), [&](AlifeData& data) { return data.ID == object->ID; });
		if (objects_active.end() != IT)
			objects_active.erase(IT);

		if (Phase() != GAME_PHASE_INPROGRESS)
			return;

		NET_Packet packet;
		packet.w_begin(M_GAMEMESSAGE);
		packet.w_u32(GAME_EVENT_OBJECTS_UPDATE);
		packet.w_u8(2);
		packet.w_u16(object->ID);
		SendedBytes += packet.B.count;
		server().SendBroadcast(server().GetServerClient()->ID, packet, net_flags(true));
	}
}
