#pragma once

#include "game_sv_mp.h"
#include "../xrEngine/pure_relcase.h"
#include "server_progress_saver.h"
#include "../jsonxx/jsonxx.h"
#include <fstream>
#include <sstream>
#include <string>

#include "xrServer.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "game_cl_base.h"

class CProgressSaver;

struct AlifeData
{
	CSE_ALifeDynamicObject* alife_object = nullptr;
	u32 dwTime = 0;
	u16 ID = 0;
};

class game_sv_freemp : public game_sv_mp, private pure_relcase
{
	typedef game_sv_mp inherited;

protected:
	CALifeSimulator* m_alife_simulator;
public:

	CProgressSaver* Saver = nullptr;

	struct inventory_boxes
	{
		CSE_Abstract* entity = nullptr;
		bool loaded = false;
	};
	xr_map<u16, inventory_boxes> inventory_boxes_cse;

	xr_map<u16, CSE_Abstract*> phy_objects_cse;

	virtual		void				OnAlifeCreate(CSE_Abstract* E);
	virtual		void				OnObjectsCreate(CSE_Abstract* E);

									game_sv_freemp();
	virtual							~game_sv_freemp();
	
	virtual		void				Create(shared_str &options);


	virtual		bool				UseSKin() const { return false; }

	virtual		LPCSTR				type_name() const { return "freemp"; };
				void				ChangeGameTime(u32 day, u32 hour, u32 minute);
				void __stdcall		net_Relcase(CObject* O) {};

	// helper functions
				void				AddMoneyToPlayer(game_PlayerState* ps, s32 amount);
				void				SetMoneyToPlayer(game_PlayerState* ps, s32 amount);
				void				SpawnItemToActor(u16 actorId, LPCSTR name);
				void				SpawnItem(LPCSTR section, u16 parent_id);
				CSE_Abstract*					SpawnItemToActorReturn(u16 actorId, LPCSTR name);
				bool							SpawnItemToPos(LPCSTR section, Fvector3 position);
				void TeleportPlayerTo(ClientID clientID, Fvector& pos, Fvector& angle);
				void TeleportPlayerTo(ClientID clientID, Fvector& pos);

	virtual		void				OnPlayerReady(ClientID id_who);
	virtual		void				OnPlayerConnect(ClientID id_who);
	virtual		void				OnPlayerConnectFinished(ClientID id_who);
	virtual		void				OnPlayerDisconnect(ClientID id_who, LPSTR Name, u16 GameID, u16 StaticID) override;

	virtual		void				OnPlayerKillPlayer(game_PlayerState* ps_killer, game_PlayerState* ps_killed, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA);

	virtual		void				OnEvent(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender);

	virtual		void				Update();
	virtual		ALife::_TIME_ID		GetStartGameTime();
	virtual		ALife::_TIME_ID		GetGameTime();

	virtual		ALife::_TIME_ID		GetEnvironmentGameTime();

	virtual		BOOL				OnTouch(u16 eid_who, u16 eid_what, BOOL bForced = FALSE);

	virtual		void				on_death(CSE_Abstract* e_dest, CSE_Abstract* e_src);

	virtual		void				OnPlayerTrade(NET_Packet &P, ClientID const & clientID);
	virtual		void				OnTransferMoney(NET_Packet &P, ClientID const & clientID);

	virtual		void				RespawnPlayer(ClientID id_who, bool NoSpectator);

#pragma region Dynamic Weather System
				void				DynamicWeatherUpdate();

				u32			nowday;
				bool		weather_will_change = false;
				bool		first_update = false;
				bool		need_change_weather = false;
	///////////Dynamic Weather /////////////
#pragma endregion

#pragma region InventoryBox Dynamic Respawn System
	jsonxx::Object m_spawn_config_json;
	jsonxx::Object m_inventory_box_config_json;
	string_path m_spawn_config_path;
	string_path m_inventory_box_config_path;
				void				   SpawnInvBoxesItems(CSE_ALifeInventoryBox* box);
				void				   OnStartSpawnInvBoxesItems(CSE_ALifeInventoryBox* box);
								   
	 u32		BoxResawnTimer = 0;
				void				   DynamicBoxFileCreate();
				void				   DynamicBoxUpdate();
#pragma endregion

#pragma region Dynamic Music System
				void				   DynamicMusicUpdate();
				void				   DynamicMusicFileCreate();
				void				   MusicPlay(CSE_ALifeObjectPhysic* obj, int pass, int obj_num);
	jsonxx::Object m_music_config_json;
	string_path m_music_config_path;
	int			numb = 0;
	int			obj_count = 4;
	int			MusicCount = 0;
	ref_sound	snd;
	u32			lenght = 0;
	bool		need_next_snd = true;
	int			i = 0;
	bool		first_play = true;
	bool		need_stop_music = false;
	u32			stop_timer = 0;
#pragma endregion


				void				UpdateAlifeData();
				void				WriteAlifeObjectsToClient(ClientID client_id);
				void				UpdateAlifeObjects();
				void				UpdateAlifeObjectsPOS();
				void				RegisterUpdateAlife(CSE_ALifeDynamicObject* object, bool reg);

		u32					last_alife_update_time = 0;
		u32					last_alife_update_time_pos = 0;
		xr_map<ClientID, bool>		map_alife_sended;
		xr_vector<AlifeData>		objects_active;

	virtual		void				assign_RP(CSE_Abstract* E, game_PlayerState* ps_who);

				void				set_account_nickname(LPCSTR login, LPCSTR password, LPCSTR new_nickname, u32 team);
				int					get_account_data(LPCSTR login, LPCSTR password, bool& leader, u8& team_leading);
				void				ChangeTeamJson(LPCSTR login, LPCSTR password, u8 team);

	void RecivePdaChatMSG(NET_Packet& P, ClientID& sender);
	void RecivePdaContactMSG(NET_Packet& P, ClientID& sender);

	virtual	bool change_level(NET_Packet& net_packet, ClientID sender);
	void restart_simulator(LPCSTR saved_game_name);
	void save_game(NET_Packet& net_packet, ClientID sender);
	bool load_game(NET_Packet& net_packet, ClientID sender);

	virtual		float				GetEnvironmentGameTimeFactor();
	virtual		void				SetEnvironmentGameTimeFactor(const float fTimeFactor);

	// SQUAD 

	virtual		void	join_player_in_squad(NET_Packet& packet, u16 id);
	virtual		void	delete_player_from_squad(NET_Packet& packet, u16 id);
	virtual		void	delete_player_from_squad(u16 id);
	virtual		void	make_player_squad_leader(NET_Packet& packet, u16 id);
	 
	struct MP_Squad
	{
		xr_vector<game_PlayerState*> players;
		ClientID squad_leader_cid;
		u16 id;
		shared_str current_quest = "";
		u16 current_map_point;
	};
	xr_vector<MP_Squad*> mp_squads;

	MP_Squad* m_lobby_squad;


	MP_Squad* create_squad(game_PlayerState* ps);
	void delete_squad(u16 squad_id);
	MP_Squad* find_squad_by_squadid(u16 id);
	bool check_player_in_squad(game_PlayerState* ps, MP_Squad* squad);
	void SendMpSuqadToMembers(MP_Squad* squad);
	void delete_player_from_player_list(MP_Squad* squad, game_PlayerState* pPlayer);
	void find_new_squad_leader(u16 squad_id);

};
extern int		box_respawn_time;
extern BOOL		set_next_music;
extern BOOL		af_debug_loggining;

extern u32 SendedBytes;
extern u32 SyncAlifeObjects;
extern int SyncAlifeCount;