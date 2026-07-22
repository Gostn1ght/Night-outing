#include "stdafx.h"
#include "UIAdminSpawner.h"
#include "ui/UIXmlInit.h"
#include "object_broker.h"
#include "ui/UIStatic.h"
#include "ui/UI3tButton.h"
#include "ui/UIListBox.h"
#include "ui/UIListBoxItem.h"
#include "ui/UIComboBox.h"
#include "ui/UITabControl.h"
#include "ui/UIScrollView.h"
#include "ui/UIEditBox.h"
#include "ui/UIHelper.h"
#include "Level.h"
#include "Actor.h"
#include "../xrEngine/xr_ioconsole.h"
#include <dinput.h>
#include "string_table.h"
#include "ui/UIInventoryUtilities.h"

const float RESIZE_GRIP_SIZE = 20.0f;
const float MIN_WINDOW_SIZE = 200.0f;

CUIAdminSpawner::CUIAdminSpawner()
{
	xml_doc = nullptr;

	m_sActiveSection = "";
	active_tab_dialog = "";
	m_bDragging = false;
	m_bResizing = false;

	m_pBack = xr_new<CUIStatic>();
	m_pBack->SetAutoDelete(true);
	AttachChild(m_pBack);

	m_pBackForDescr = xr_new<CUIStatic>();
	m_pBackForDescr->SetAutoDelete(true);
	AttachChild(m_pBackForDescr);

	m_pTabControl = xr_new<CUITabControl>();
	m_pTabControl->SetAutoDelete(true);
	AttachChild(m_pTabControl);

	item_icon = nullptr;
	itm_name = nullptr;
	itm_desc = nullptr;
	itm_weight = nullptr;
	itm_cost = nullptr;

	m_pSpawnQuantityEditBox = nullptr;

	item_icon = xr_new<CUIStatic>();
	item_icon->SetAutoDelete(true);
	AttachChild(item_icon);

	itm_name = xr_new<CUIStatic>();
	itm_name->SetAutoDelete(true);
	AttachChild(itm_name);

	itm_desc = xr_new<CUITextWnd>();
	itm_desc->SetAutoDelete(true);
	AttachChild(itm_desc);

	itm_weight = xr_new<CUIStatic>();
	itm_weight->SetAutoDelete(true);
	AttachChild(itm_weight);

	itm_cost = xr_new<CUIStatic>();
	itm_cost->SetAutoDelete(true);
	AttachChild(itm_cost);

	items_list_box = xr_new<CUIListBox>();
	items_list_box->SetAutoDelete(true);
	AttachChild(items_list_box);

	scroll_v = xr_new<CUIScrollView>();
	scroll_v->SetAutoDelete(true);
	AttachChild(scroll_v);

	m_pClose = xr_new<CUI3tButton>();
	m_pClose->SetAutoDelete(true);
	AttachChild(m_pClose);

	m_pSpawnItem = xr_new<CUI3tButton>();
	m_pSpawnItem->SetAutoDelete(true);
	AttachChild(m_pSpawnItem);

	m_pIXraySearcherBack = xr_new<CUIStatic>();
	m_pIXraySearcherBack->SetAutoDelete(true);
	AttachChild(m_pIXraySearcherBack);

	m_pLogBack = xr_new<CUIStatic>();
	m_pLogBack->SetAutoDelete(true);
	AttachChild(m_pLogBack);

	m_pWeatherBack = xr_new<CUIStatic>();
	m_pWeatherBack->SetAutoDelete(true);
	AttachChild(m_pWeatherBack);

	m_pPlayerOptionsBack = xr_new<CUIStatic>();
	m_pPlayerOptionsBack->SetAutoDelete(true);
	AttachChild(m_pPlayerOptionsBack);

	m_pAnomalyOptionsBack = xr_new<CUIStatic>();
	m_pAnomalyOptionsBack->SetAutoDelete(true);
	AttachChild(m_pAnomalyOptionsBack);

	m_pStatsBack = xr_new<CUIStatic>();
	m_pStatsBack->SetAutoDelete(true);
	AttachChild(m_pStatsBack);

	m_pIXraySearcherBack = nullptr;
	m_pIXraySearcherInput = nullptr;
	m_pIXraySearcherButton = nullptr;
	m_pIXraySearcherResults = nullptr;

	m_pLogBack = nullptr;
	m_pLogScrollView = nullptr;

	m_pWeatherBack = nullptr;
	m_pWeatherComboBox = nullptr;
	m_pApplyWeatherButton = nullptr;

	m_pPlayerOptionsBack = nullptr;
	m_pGodModeButton = nullptr;
	m_pNoclipButton = nullptr;

	m_pAnomalyOptionsBack = nullptr;
	m_pAnomalyComboBox = nullptr;
	m_pSpawnAnomalyButton = nullptr;

	m_pStatsBack = nullptr;
	m_pStatsScrollView = nullptr;

	Init();
}

CUIAdminSpawner::~CUIAdminSpawner()
{
	xr_delete(xml_doc);

	xr_delete(m_pIXraySearcherBack);
	xr_delete(m_pIXraySearcherInput);
	xr_delete(m_pIXraySearcherButton);
	xr_delete(m_pIXraySearcherResults);

	xr_delete(m_pLogBack);
	xr_delete(m_pLogScrollView);

	xr_delete(m_pWeatherBack);
	xr_delete(m_pWeatherComboBox);
	xr_delete(m_pApplyWeatherButton);

	xr_delete(m_pPlayerOptionsBack);
	xr_delete(m_pGodModeButton);
	xr_delete(m_pNoclipButton);

	xr_delete(m_pAnomalyOptionsBack);
	xr_delete(m_pAnomalyComboBox);
	xr_delete(m_pSpawnAnomalyButton);

	xr_delete(m_pStatsBack);
	xr_delete(m_pStatsScrollView);
}

void CUIAdminSpawner::FillSpawnerList()
{
	items_list_box->Clear();

	// Hide or show items_list_box and related item description elements based on the active tab
	bool show_item_list = true;
	if (active_tab_dialog == "stats_sect_for_spawner" ||
		active_tab_dialog == "cheats_sect_for_spawner" ||
		active_tab_dialog == "misc_sect_for_spawner")
	{
		show_item_list = false;
	}

	items_list_box->SetVisible(show_item_list);
	scroll_v->SetVisible(show_item_list);
	if (item_icon) item_icon->SetVisible(show_item_list);
	if (itm_name) itm_name->SetVisible(show_item_list);
	if (itm_desc) itm_desc->SetVisible(show_item_list);
	if (itm_weight) itm_weight->SetVisible(show_item_list);
	if (itm_cost) itm_cost->SetVisible(show_item_list);
	if (m_pSpawnQuantityEditBox) m_pSpawnQuantityEditBox->SetVisible(show_item_list);
	if (m_pSpawnItem) m_pSpawnItem->SetVisible(show_item_list);

	if (!show_item_list)
		return; // No items to list for these tabs

	if (!pSettings->section_exist(active_tab_dialog))
		return;

	if (active_tab_dialog == "")
		return;

	// Handle "all" tab
	if (active_tab_dialog == "all_sect_for_spawner")
	{
		// This will be more complex. For now, let's just make it do nothing if this section doesn't exist.
		// A proper implementation would iterate through all item categories.
		// For now, it will just rely on `pSettings->section_exist`
	}

	CInifile::Sect& sect = pSettings->r_section(active_tab_dialog);
	CInifile::SectCIt it_ = sect.Data.begin();
	CInifile::SectCIt it_e_ = sect.Data.end();

	for (; it_ != it_e_; ++it_)
	{
		string512 tmp_string;
		xr_sprintf(tmp_string, "%s", it_->first.c_str());
		AddItemsToSpawnerList(it_->first);
	}
}

void CUIAdminSpawner::AddItemsToSpawnerList(shared_str section)
{
	xr_string name_translate = CStringTable().translate(pSettings->r_string(section, "inv_name")).c_str();
	if (name_translate.size() > 90)
		name_translate.substr(0, 87) + "...";

	CUIListBoxItem* itm = items_list_box->AddTextItem(name_translate.c_str());
	translate_and_sect[section] = name_translate.c_str();
	for (auto& pair : translate_and_sect)
	{
		pair.second = CStringTable().translate(pSettings->r_string(pair.first, "inv_name")).c_str();
	}
}

void CUIAdminSpawner::SetActiveSubdialogSpawner(const shared_str& section)
{
	if (m_sActiveSection == section)
		return;

	if (section == "all")
		active_tab_dialog = "all_sect_for_spawner"; // Placeholder
	else if (section == "stats")
		active_tab_dialog = "stats_sect_for_spawner"; // Placeholder
	else if (section == "monsters")
		active_tab_dialog = "monster_sect_for_spawner"; // Placeholder
	else if (section == "weapons")
		active_tab_dialog = "weapon_sect_for_spawner";
	else if (section == "ammos")
		active_tab_dialog = "ammo_sect_for_spawner";
	else if (section == "addons")
		active_tab_dialog = "addon_sect_for_spawner";
	else if (section == "outfits")
		active_tab_dialog = "outfit_sect_for_spawner";
	else if (section == "artefacts")
		active_tab_dialog = "artefact_sect_for_spawner";
	else if (section == "items")
		active_tab_dialog = "items_sect_for_spawner";
	else if (section == "misc")
		active_tab_dialog = "misc_sect_for_spawner"; // Placeholder
	else if (section == "cheats")
		active_tab_dialog = "cheats_sect_for_spawner"; // Placeholder
	else if (section == "quests")
		active_tab_dialog = "quest_sect_for_spawner";
	else if (section == "uniques")
		active_tab_dialog = "unique_sect_for_spawner";

	m_sActiveSection = section;
	FillSpawnerList();
}

void CUIAdminSpawner::Init()
{
	if (!xml_doc)
		xml_doc = xr_new<CUIXml>();

	xml_doc->Load(CONFIG_PATH, UI_PATH, "ui_mp_adm_spawner.xml");

	CUIXmlInit::InitWindow(*xml_doc, "ui_mp_adm_spawner", 0, this);
	CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:background", 0, m_pBack);
	CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:background_description", 0, m_pBackForDescr);
	CUIXmlInit::InitTabControl(*xml_doc, "ui_mp_adm_spawner:tab_control", 0, m_pTabControl);
	CUIXmlInit::InitScrollView(*xml_doc, "ui_mp_adm_spawner:scroll_v", 0, scroll_v);

	m_pTabControl->SetActiveTab("weapons");
	active_tab_dialog = "weapon_sect_for_spawner";

	CUIXmlInit::InitListBox(*xml_doc, "ui_mp_adm_spawner:skin_list", 0, items_list_box);
	FillSpawnerList();

	CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:itm_icon", 0, item_icon);
	CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:itm_name", 0, itm_name);
	CUIXmlInit::InitTextWnd(*xml_doc, "ui_mp_adm_spawner:itm_desc", 0, itm_desc);
	CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:itm_weight", 0, itm_weight);
	CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:itm_cost", 0, itm_cost);

	CUIXmlInit::Init3tButton(*xml_doc, "ui_mp_adm_spawner:spawn_item_button", 0, m_pSpawnItem);
	CUIXmlInit::Init3tButton(*xml_doc, "ui_mp_adm_spawner:close_button", 0, m_pClose);

	CUIXmlInit::InitEditBox(*xml_doc, "ui_mp_adm_spawner:spawn_quantity_edit_box", 0, m_pSpawnQuantityEditBox);
}

void CUIAdminSpawner::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	switch (msg)
	{
	case TAB_CHANGED:
	{
		if (pWnd == m_pTabControl)
		{
			SetActiveSubdialogSpawner(m_pTabControl->GetActiveId());
		}
		break;
	}
	case LIST_ITEM_SELECT:
	{
		DrawSelectedItem();
		break;
	}
	case BUTTON_CLICKED:
	{
		if (pWnd == m_pClose)
			HideDialog();
		else if (pWnd == m_pSpawnItem)
			SpawnItem();
		break;
	}
	};
}

void CUIAdminSpawner::SpawnItem()
{
	if (items_list_box->GetSize() == 0) return;

	CUIListBoxItem* itm = items_list_box->GetSelectedItem();
	if (!itm)
		return;

	string128 section = "";
	xr_strcpy(section, itm->GetText());

	shared_str true_sect = nullptr;
	for (auto& pair : translate_and_sect)
	{
		if (0 == xr_strcmp(pair.second, section))
		{
			true_sect = pair.first;
		}
	}

	if (!pSettings->section_exist(true_sect))
		Debug.fatal(DEBUG_INFO, "SPAWNER: section doesnt exists");

	NET_Packet P;
	Game().u_EventGen(P, GE_GAME_EVENT, Game().local_player->GameID);
	P.w_u16(GAME_EVENT_SPAWNER_SPAWN_ITEM);
	P.w_stringZ(true_sect);
	Game().u_EventSend(P);
}

void CUIAdminSpawner::DrawSelectedItem()
{
	CUIListBoxItem* itm = items_list_box->GetSelectedItem();
	if (!itm) return;

	if (itm_name == nullptr)
	{
		itm_name = xr_new<CUIStatic>();
		itm_name->SetAutoDelete(true);
		scroll_v->AddWindow(itm_name, true);
		CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:itm_name", 0, itm_name);
	}

	if (itm_weight == nullptr)
	{
		itm_weight = xr_new<CUIStatic>();
		itm_weight->SetAutoDelete(true);
		scroll_v->AddWindow(itm_weight, true);
		CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:itm_weight", 0, itm_weight);
	}

	if (itm_cost == nullptr)
	{
		itm_cost = xr_new<CUIStatic>();
		itm_cost->SetAutoDelete(true);
		scroll_v->AddWindow(itm_cost, true);
		CUIXmlInit::InitStatic(*xml_doc, "ui_mp_adm_spawner:itm_cost", 0, itm_cost);
	}

	if (itm_desc == nullptr)
	{
		itm_desc = xr_new<CUITextWnd>();
		itm_desc->SetAutoDelete(true);
		scroll_v->AddWindow(itm_desc, true);
		CUIXmlInit::InitTextWnd(*xml_doc, "ui_mp_adm_spawner:itm_desc", 0, itm_desc);
	}

	string256 section, name = "";
	LPCSTR desc = "";
	float weight;
	u32 cost;
	shared_str true_sect = nullptr;

	xr_strcpy(section, itm->GetText());
	for (auto& pair : translate_and_sect)
	{
		if (0 == xr_strcmp(pair.second, section))
		{
			true_sect = pair.first;
		}
	}

	if (!true_sect.size())
		Debug.fatal(DEBUG_INFO, "section in spawner is empty");

	xr_strcpy(name, READ_IF_EXISTS(pSettings, r_string, true_sect, "inv_name", "<No Name>"));
	weight = READ_IF_EXISTS(pSettings, r_float, true_sect, "inv_weight", 0);
	cost = READ_IF_EXISTS(pSettings, r_u32, true_sect, "cost", 0);
	desc = READ_IF_EXISTS(pSettings, r_string, true_sect, "description", "<No description>");

	string256 text_name = "";
	string4096 text_descr = "";
	string128 text_weight, text_cost = "";

	xr_sprintf(text_descr, "��������: %s", *CStringTable().translate(desc));
	xr_sprintf(text_weight, "���: %.2f ��.", weight);
	xr_sprintf(text_cost, "����: %d ���.", cost);
	xr_sprintf(text_name, "������: %s", true_sect.c_str());

	itm_name->TextItemControl()->SetText(text_name);
	itm_name->TextItemControl()->SetTextColor(color_rgba(255, 255, 255, 255));

	itm_weight->TextItemControl()->SetText(text_weight);
	itm_weight->TextItemControl()->SetTextColor(color_rgba(255, 255, 255, 255));

	itm_cost->TextItemControl()->SetText(text_cost);
	itm_cost->TextItemControl()->SetTextColor(color_rgba(255, 255, 255, 255));

	itm_desc->SetWidth(scroll_v->GetDesiredChildWidth());
	itm_desc->SetTextComplexMode(true);
	itm_desc->SetText(text_descr);
	itm_desc->AdjustHeightToText();
	scroll_v->ScrollToBegin();

	Frect texture_rect;
	texture_rect.x1 = pSettings->r_float(true_sect, "inv_grid_x") * INV_GRID_WIDTH;
	texture_rect.y1 = pSettings->r_float(true_sect, "inv_grid_y") * INV_GRID_HEIGHT;
	texture_rect.x2 = pSettings->r_float(true_sect, "inv_grid_width") * INV_GRID_WIDTH;
	texture_rect.y2 = pSettings->r_float(true_sect, "inv_grid_height") * INV_GRID_HEIGHT;

	float pw = (texture_rect.x2 / 5) * 4;
	float ph = (texture_rect.y2 / 5) * 4;
	float px = xml_doc->ReadAttribFlt("ui_mp_adm_spawner:itm_icon", 0, "x") - (pw / 2);
	float py = xml_doc->ReadAttribFlt("ui_mp_adm_spawner:itm_icon", 0, "y") - (ph / 2);

	if (item_icon == nullptr)
	{
		item_icon = xr_new<CUIStatic>();
		item_icon->SetWindowName("pict");
		item_icon->SetAutoDelete(true);
		AttachChild(item_icon);
	}

	item_icon->InitTexture("ui\\ui_icon_equipment");
	item_icon->SetTextureRect(Frect().set(texture_rect.x1, texture_rect.y1, texture_rect.x2 + texture_rect.x1, texture_rect.y2 + texture_rect.y1));
	item_icon->SetWndRect(Frect().set(px, py, pw + px, ph + py));
	item_icon->SetStretchTexture(true);
}

bool CUIAdminSpawner::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
	if (keyboard_action == WINDOW_KEY_PRESSED)
	{
		if (dik == DIK_ESCAPE || is_binded(kQUIT, dik))
			HideDialog();
		else if (dik == DIK_RETURN)
			SpawnItem();

		return true;
	}

	return CUIDialogWnd::OnKeyboardAction(dik, keyboard_action);
}

bool CUIAdminSpawner::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
	if (inherited::OnMouseAction(x, y, mouse_action))
		return true;

	switch (mouse_action)
	{
	case WINDOW_LBUTTON_DOWN:
		{
			Frect rect;
			GetWndRect(rect);

			// Check for resize grip (bottom-right corner)
			if (x >= rect.width() - RESIZE_GRIP_SIZE && y >= rect.height() - RESIZE_GRIP_SIZE)
			{
				m_bResizing = true;
				m_vResizeStartPos.set(x, y);
				m_vWindowStartSize = GetWndSize();
				return true;
			}

			// If not resizing, then check for dragging
			m_bDragging = true;
			m_vDragStartPos.set(x, y);
			m_vWindowStartPos = GetWndPos();
			return true;
		}
	case WINDOW_LBUTTON_UP:
		{
			m_bDragging = false;
			m_bResizing = false;
			return true;
		}
	};

	return false;
}

bool CUIAdminSpawner::OnMouseMove(float x, float y)
{
	inherited::OnMouseMove();

	if (m_bDragging)
	{
		Fvector2 current_mouse_pos;
		current_mouse_pos.set(x, y);
		Fvector2 delta;
		delta.sub(current_mouse_pos, m_vDragStartPos);

		Fvector2 new_pos;
		new_pos.add(m_vWindowStartPos, delta);
		SetWndPos(new_pos);
		return true;
	}
	else if (m_bResizing)
	{
		Fvector2 current_mouse_pos;
		current_mouse_pos.set(x, y);
		Fvector2 delta;
		delta.sub(current_mouse_pos, m_vResizeStartPos);

		Fvector2 new_size;
		new_size.add(m_vWindowStartSize, delta);

		// Enforce minimum size
		if (new_size.x < MIN_WINDOW_SIZE) new_size.x = MIN_WINDOW_SIZE;
		if (new_size.y < MIN_WINDOW_SIZE) new_size.y = MIN_WINDOW_SIZE;

		SetWndSize(new_size);
		return true;
	}

	return false;
}
