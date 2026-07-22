#pragma once

#include "ui/UIDialogWnd.h"

class CUIStatic;
class CUITabControl;
class CUIXml;
class CUIWindow;
class CUI3tButton;
class CUIListBox;
class CUIScrollView;
class CUITextWnd;
class CUIEditBox;
class CUIComboBox;

class CUIAdminSpawner : public CUIDialogWnd
{
private:
	typedef CUIWindow	inherited;

	CUIXml* xml_doc;
	CUIStatic* m_pBack;
	CUIStatic* m_pBackForDescr;
	CUITabControl* m_pTabControl;
	CUIListBox* items_list_box;
	CUIStatic* item_icon;
	CUIScrollView* scroll_v;
	CUIStatic* itm_name;
	CUITextWnd* itm_desc;
	CUIStatic* itm_weight;
	CUIStatic* itm_cost;

	CUIEditBox* m_pSpawnQuantityEditBox;

	// Dragging variables
	bool m_bDragging;
	Fvector2 m_vDragStartPos;
	Fvector2 m_vWindowStartPos;

	// Resizing variables
	bool m_bResizing;
	Fvector2 m_vResizeStartPos;
	Fvector2 m_vWindowStartSize;

	CUIStatic* m_pIXraySearcherBack;
	CUIEditBox* m_pIXraySearcherInput;
	CUI3tButton* m_pIXraySearcherButton;
	CUIListBox* m_pIXraySearcherResults;

	CUIStatic* m_pLogBack;
	CUIScrollView* m_pLogScrollView;

	CUIStatic* m_pWeatherBack;
	CUIComboBox* m_pWeatherComboBox;
	CUI3tButton* m_pApplyWeatherButton;

	CUIStatic* m_pPlayerOptionsBack;
	CUI3tButton* m_pGodModeButton;
	CUI3tButton* m_pNoclipButton;

	CUIStatic* m_pAnomalyOptionsBack;
	CUIComboBox* m_pAnomalyComboBox;
	CUI3tButton* m_pSpawnAnomalyButton;

	CUIStatic* m_pStatsBack;
	CUIScrollView* m_pStatsScrollView;

	CUI3tButton* m_pClose;
	CUI3tButton* m_pSpawnItem;

	shared_str m_sActiveSection;
	LPCSTR active_tab_dialog;
	xr_map<shared_str, LPCSTR> translate_and_sect;
public:
	virtual void FillSpawnerList();
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	void SetActiveSubdialogSpawner(const shared_str& section);

	void DrawSelectedItem();
	void SpawnItem();
	void AddItemsToSpawnerList(shared_str section);
	CUIAdminSpawner();
	virtual ~CUIAdminSpawner();
	void Init();
	virtual bool OnKeyboardAction(int dik, EUIMessages keyboard_action);
	virtual bool OnMouseAction(float x, float y, EUIMessages mouse_action);
	virtual bool OnMouseMove(float x, float y);
};
