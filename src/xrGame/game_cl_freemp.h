#pragma once
#include "game_cl_mp.h"

class CVoiceChat;
class CUIGameFMP;

class game_cl_freemp :public game_cl_mp
{
private:
	typedef game_cl_mp inherited;


public:
	CVoiceChat* m_pVoiceChat = nullptr;
	CUIGameFMP* m_game_ui;
			game_cl_freemp();
	virtual	~game_cl_freemp();

	virtual LPCSTR		GetTeamColor(u32 /*team*/) const { return "%c[255,255,240,190]"; }
	virtual u32				GetTeamColor_u32(u32 /*team*/) const { return color_rgba(255, 240, 190, 255); }

	virtual CUIGameCustom* createGameUI();
	virtual void SetGameUI(CUIGameCustom*);

	virtual void TranslateGameMessage(u32 msg, NET_Packet& P);

	virtual	void net_import_state(NET_Packet& P);
	virtual	void net_import_update(NET_Packet& P);
	
	virtual void shedule_Update(u32 dt);

	virtual void OnRender();

	virtual	bool OnKeyboardPress(int key);
	virtual	bool OnKeyboardRelease(int key);

	virtual LPCSTR GetGameScore(string32&	score_dest);
	virtual bool Is_Rewarding_Allowed()  const { return false; };

	virtual void OnConnected();

	virtual void OnScreenResolutionChanged();

	void OnChatMessage(NET_Packet* P);

	void SendHelloMsg();

	// --- MP feature client handlers (a-life sync / PDA / squads) ---
	// Client-side mirror of an a-life object streamed from the server.
	struct MPClientAlifeObj
	{
		u16			id = 0;
		shared_str	name;
		Fvector		pos = { 0, 0, 0 };
	};

	// Client-side mirror of the player's MP squad.
	struct MPClientSquad
	{
		ClientID		leader;
		u16				id = 0;
		u16				map_point = 0;
		shared_str		quest;
		xr_vector<u16>	members;
	};

private:
	void OnVoiceMessage(NET_Packet* P);

	// Handlers for the freemp game-message events (see TranslateGameMessage).
	void OnPlayerTeleport(NET_Packet& P);
	void OnAlifeObjectSpawn(NET_Packet& P);
	void OnAlifeObjectUpdate(NET_Packet& P);
	void OnPdaChatMessage(NET_Packet& P);
	void OnPdaContactMessage(NET_Packet& P);
	void OnSquadNotify(NET_Packet& P);
	void OnSquadRespondInvite(NET_Packet& P);

	// Show a short PDA news/notification message using the existing UI.
	void ShowPdaNotification(LPCSTR caption, LPCSTR text, LPCSTR icon);

	bool adm_wallhack = false;

	// Client-side mirror state populated from server events.
	xr_map<u16, MPClientAlifeObj>	m_client_alife_objects;
	xr_vector<u32>					m_pda_contacts;
	MPClientSquad					m_client_squad;
};

