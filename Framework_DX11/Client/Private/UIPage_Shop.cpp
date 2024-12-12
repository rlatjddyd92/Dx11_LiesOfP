#include "stdafx.h"
#include "..\Public\UIPage_Shop.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Shop::CUIPage_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Shop::CUIPage_Shop(const CUIPage_Shop& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Shop::Initialize_Prototype()
{
	// 모든 UIPage는 클론 안 함, 여기서 모든 세팅 끝내기 

	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_vecPageAction.resize(_int(PAGEACTION::ACTION_END));

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPage_Shop::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Shop::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Shop::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pScroll_Shop == nullptr)
	{
		SCROLL* pScroll_Shop = new SCROLL;
		pScroll_Shop->Initialize_Scroll(m_vecPart[_int(PART_GROUP::SHOP_Data_Area)], m_vecPart[_int(PART_GROUP::SHOP_Scroll_Bar)], SCROLL_AREA::SCROLL_SHOP);
		m_pScroll_Shop = pScroll_Shop;
	}
}

void CUIPage_Shop::Late_Update(_float fTimeDelta)
{
	Update_Tab(fTimeDelta);

	if (m_pScroll_Shop->bIsActive)
		Update_Scroll(fTimeDelta);

	Update_Cell(fTimeDelta);
	Update_Focus(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUIPage_Shop::Render()
{
	return S_OK;
}

void CUIPage_Shop::OpenAction()
{
	__super::OpenAction();
	Setting_SellTab();
	Setting_BuyTab();
}

void CUIPage_Shop::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Shop::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Action_Tab(fTimeDelta);
	Action_Scroll(fTimeDelta);

	if (!m_bIsItem_Popup)
		Action_Cell(fTimeDelta);
	else
		Action_Popup(fTimeDelta);


	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Shop::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	m_vec_Group_Ctrl.resize(_int(PART_GROUP::GROUP_END));

	for (auto& iter : m_vec_Group_Ctrl)
		iter = new UG_CTRL;

	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		if (m_vecPart[i]->iGroupIndex != -1)
			m_vec_Group_Ctrl[m_vecPart[i]->iGroupIndex]->PartIndexlist.push_back(i);
	}

	m_bRender = false;

	return S_OK;
}

void CUIPage_Shop::Action_Tab(_float fTimeDelta)
{
	_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Mouse_0))->fPosition, __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Mouse_0))->fSize);
	_bool bClick = KEY_TAP(KEY::LBUTTON);

	if (vMouse.x != -1.f)
	{
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Fx_0))->bRender = true;
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Fx_1))->bRender = false;

		if (bClick)
			if (m_iNowTab != 0)
			{
				m_iNowTab = 0;
				Setting_BuyTab();
			}
	}

	vMouse = GET_GAMEINTERFACE->CheckMouse(__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Mouse_1))->fPosition, __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Mouse_1))->fSize);
	if (vMouse.x != -1.f)
	{
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Fx_0))->bRender = false;
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Fx_1))->bRender = true;

		if (bClick)
			if (m_iNowTab != 1)
			{
				m_iNowTab = 1;
				Setting_SellTab();
			}
	}
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Coin_Num))->strText = to_wstring(GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat().iErgo);
}

void CUIPage_Shop::Action_Scroll(_float fTimeDelta)
{
	if (!m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		if (m_pScroll_Shop->bIsBarMoving)
		{
			if (KEY_HOLD(KEY::LBUTTON))
			{
				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);
				m_pScroll_Shop->Bar_Moving(_float(ptMouse.y));
			}
			else
				m_pScroll_Shop->End_Bar_Moving();
		}
		else
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Scroll_Bar))->fPosition, __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Scroll_Bar))->fSize);

			if (vMouse.x != -1.f)
				if (KEY_TAP(KEY::LBUTTON))
				{
					m_pScroll_Shop->Start_Bar_Moving(vMouse.y);
				}
		}
	}

	if (m_iNowTab == 0)
		for (auto& iter : m_vecBuy_RenderInfo)
			iter->vPos.y -= m_pScroll_Shop->fData_Offset_Y;

	if (m_iNowTab == 1)
		for (auto& iter : m_vecSell_RenderInfo)
			iter->vPos.y -= m_pScroll_Shop->fData_Offset_Y;

	__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Scroll_Bar))->fRatio = m_pScroll_Shop->fScroll_Ratio;
	__super::UpdatePart_ByIndex(__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::SHOP_Scroll_Bar)), fTimeDelta);
}

void CUIPage_Shop::Action_Cell(_float fTimeDelta)
{
	_bool bClick = KEY_TAP(KEY::RBUTTON);

	_int iNowIndex = -1;

	if (m_iNowTab == 0)
	{
		for (auto& iter : m_vecBuy_RenderInfo)
		{
			++iNowIndex;
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(iter->vPos, __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Static))->fSize);
			if (vMouse.x != -1.f)
			{
				Action_Focus(fTimeDelta, iter->vPos);
				m_iNowCell = iNowIndex;
				if (bClick)
				{
					CItem_Manager::SHOP* pShop = GET_GAMEINTERFACE->Get_ShopData()[iter->iIndexShop];
					const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Item_Origin_Spec(pShop->iIndex);

					if (pShop != nullptr)
					{
						m_iNowCount = 0;
						m_iNowPrice = 0;
						GET_GAMEINTERFACE->Show_ItemPopup(TEXT("상점 구매"), pItem->strName, 1, &m_iNowCount, pShop->iCount, TEXT("소모 에르고"), pShop->iPrice_Buy, &m_iNowPrice);
						m_bIsItem_Popup = true;
					}
				}
			}
		}
	}
	else
	{
		for (auto& iter : m_vecSell_RenderInfo)
		{
			++iNowIndex;
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(iter->vPos, __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Static))->fSize);
			if (vMouse.x != -1.f)
			{
				Action_Focus(fTimeDelta, iter->vPos);
				m_iNowCell = iNowIndex;
				if (bClick)
				{
					CItem_Manager::SHOP* pShop = GET_GAMEINTERFACE->Get_ShopData()[iter->iIndexShop];

					if (iter->pInven != nullptr)
					{
						m_iNowCount = 0;
						m_iNowPrice = 0;
						GET_GAMEINTERFACE->Show_ItemPopup(TEXT("상점 판매"), iter->pInven->strName, 1, &m_iNowCount, iter->pInven->iCount, TEXT("획득 에르고"), iter->pInven->iPrice, &m_iNowPrice);
						m_bIsItem_Popup = true;
					}
				}
			}
		}
	}
}

void CUIPage_Shop::Action_Focus(_float fTimeDelta, _Vec2 vPos)
{
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Rect))->fPosition = vPos;
	if (__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fAdjust_End != vPos + __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fAdjust)
	{
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fAdjust_Start = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fPosition;
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fAdjust_End = vPos + __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fAdjust;
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fRatio = 0.f;
	}
}

void CUIPage_Shop::Action_Popup(_float fTimeDelta)
{
	if (m_iNowTab == 0)
	{
		CItem_Manager::SHOP* pShop = GET_GAMEINTERFACE->Get_ShopData()[m_vecBuy_RenderInfo[m_iNowCell]->iIndexShop];
		const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Item_Origin_Spec(pShop->iIndex);
		const CPlayer::PLAYER_STAT_INFO pOrigin = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();

		if ((m_iNowCount != 0) || (m_iNowPrice != 0))
		{
			if (m_iNowCount > pShop->iCount)
				GET_GAMEINTERFACE->Show_Popup(TEXT("구매 불가"), TEXT("구매 가능한 수량을 초과하였습니다."));
			if (m_iNowPrice > pOrigin.iErgo)
				GET_GAMEINTERFACE->Show_Popup(TEXT("구매 불가"), TEXT("보유한 에르고가 충분하지 않습니다."));
			else
				GET_GAMEINTERFACE->Buy_ShopItem(m_iNowCell);

			Reset_PopupInfo();
		}
	}
	else
	{
		const CItem_Manager::ITEM* pItem = m_vecSell_RenderInfo[m_iNowCell]->pInven;

		if ((m_iNowCount != 0) || (m_iNowPrice != 0))
		{
			if (m_iNowCount > pItem->iCount)
				GET_GAMEINTERFACE->Show_Popup(TEXT("판매 불가"), TEXT("판매 가능한 수량을 초과하였습니다."));
			else
				GET_GAMEINTERFACE->Sell_ShopItem(m_vecSell_RenderInfo[m_iNowCell]->eArray, m_vecSell_RenderInfo[m_iNowCell]->iInven_Index);

			Reset_PopupInfo();
		}
	}
}

void CUIPage_Shop::Update_Tab(_float fTimeDelta)
{
	for (_int i = 0; i < _int(PART_GROUP::SHOP_Coin_Num);++i)
		for(auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
			if (m_vecPart[iter]->bRender == true)
				__super::Input_Render_Info(*m_vecPart[iter]);
}

void CUIPage_Shop::Update_Scroll(_float fTimeDelta)
{
	for (_int i = _int(PART_GROUP::SHOP_Scroll_Line); i < _int(PART_GROUP::SHOP_Scroll_Bar); ++i)
		for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
			if (m_vecPart[iter]->bRender == true)
				__super::Input_Render_Info(*m_vecPart[iter]);
}

void CUIPage_Shop::Update_Cell(_float fTimeDelta)
{
	vector<CELL_RENDER*>& NowInfo = m_iNowTab == 0 ? m_vecBuy_RenderInfo : m_vecSell_RenderInfo;

	_int iNowIndex = -1;

	if (m_iNowTab == 1)
	{
		for (_int i = _int(PART_GROUP::SHOP_Title_Text); i < _int(PART_GROUP::SHOP_Title_Line); ++i)
			for (auto& iterPart : m_vec_Group_Ctrl[i]->PartIndexlist)
			{
				m_vecPart[iterPart]->fPosition = m_Title_Bag->vPos;

				if (i == _int(PART_GROUP::SHOP_Title_Text))
					m_vecPart[iterPart]->strText = m_Title_Bag->strTitle;

				__super::Input_Render_Info(*m_vecPart[iterPart], SCROLL_AREA::SCROLL_SHOP);
			}
	}

	for (auto& iter : NowInfo)
	{
		++iNowIndex;

		for (_int i = _int(PART_GROUP::SHOP_Cell_Static); i < _int(PART_GROUP::SHOP_Cell_Cost_Num); ++i)
			for (auto& iterPart : m_vec_Group_Ctrl[i]->PartIndexlist)
			{
				m_vecPart[iterPart]->fPosition = iter->vPos;

				if (i == _int(PART_GROUP::SHOP_Cell_Fx))
				{
					if (iNowIndex == m_iNowCell)
						__super::Input_Render_Info(*m_vecPart[iterPart], SCROLL_AREA::SCROLL_SHOP);

					continue;
				}
				else if (i == _int(PART_GROUP::SHOP_Cell_Item))
				{
					m_vecPart[iterPart]->iTexture_Index = iter->pInven->iTexture_Index;
				}
				else if (i == _int(PART_GROUP::SHOP_Cell_Count))
				{
					m_vecPart[iterPart]->strText = to_wstring(iter->pInven->iCount);
				}
				else if (i == _int(PART_GROUP::SHOP_Cell_Cost_Num))
				{
					m_vecPart[iterPart]->strText = to_wstring(iter->pInven->iPrice);
				}
				__super::Input_Render_Info(*m_vecPart[iterPart], SCROLL_AREA::SCROLL_SHOP);
			}
	}
	
	if (m_iNowTab == 1)
	{
		for (_int i = _int(PART_GROUP::SHOP_Title_Text); i < _int(PART_GROUP::SHOP_Title_Line); ++i)
			for (auto& iterPart : m_vec_Group_Ctrl[i]->PartIndexlist)
			{
				m_vecPart[iterPart]->fPosition = m_Title_Chest->vPos;

				if (i == _int(PART_GROUP::SHOP_Title_Text))
					m_vecPart[iterPart]->strText = m_Title_Chest->strTitle;

				__super::Input_Render_Info(*m_vecPart[iterPart], SCROLL_AREA::SCROLL_SHOP);
			}
	}
}

void CUIPage_Shop::Update_Focus(_float fTimeDelta)
{
	_float fRatio = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fRatio;
	_Vec2 vEnd = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fAdjust_End;
	_Vec2 vStart = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fAdjust_Start;

	fRatio += fTimeDelta;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fRatio = min(fRatio, 1.f);
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire))->fPosition = vStart + ((vEnd - vStart) * fRatio);
	
	__super::Input_Render_Info(*__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Rect)), SCROLL_AREA::SCROLL_SHOP);
	__super::Input_Render_Info(*__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_SELECT_Fire)), SCROLL_AREA::SCROLL_SHOP);
}

void CUIPage_Shop::Update_BagInfo(_float fTimeDelta)
{
	for (_int i = _int(PART_GROUP::SHOP_BoxInfo_Bag_Count_0); i < _int(PART_GROUP::SHOP_BoxInfo_Bag_Count_1); ++i)
		for (auto& iterPart : m_vec_Group_Ctrl[i]->PartIndexlist)
		{
			__super::Input_Render_Info(*m_vecPart[iterPart], SCROLL_AREA::SCROLL_SHOP);
		}
}

void CUIPage_Shop::Setting_SellTab()
{
	for (auto& iter : m_vecSell_RenderInfo)
		Safe_Delete(iter);

	m_vecSell_RenderInfo.clear();

	_float fStartY = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Data_Area))->fPosition.y - __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Data_Area))->fSize.y * 0.5f;
	_float fAdjustY = 0.f;

	if (m_Title_Bag == nullptr)
	{
		m_Title_Bag = new CELL_RENDER;
		m_Title_Bag->bIsTitle = true;
		m_Title_Bag->strTitle = TEXT("가방");
		
		fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Title_Text))->fSize.y * 0.5f;
		m_Title_Bag->vPos = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Title_Text))->fPosition;
		m_Title_Bag->vPos.y = fStartY + fAdjustY;
		fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Title_Text))->fSize.y * 0.5f;
	}

	fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fSize.y * 0.5f;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fPosition.y = fStartY + fAdjustY;
	_int iArraynum = 0;
	
	for (_int i = 0; i > _int(INVEN_ARRAY_TYPE::TYPE_END); ++i)
	{
		_int iMax = 0;
		_wstring strName = {};
		GET_GAMEINTERFACE->Get_Array_Info(INVEN_ARRAY_TYPE(i), &strName, &iMax);

		for (_int j = 0; j < iMax; ++j)
		{
			const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE(i), j);
			if (pNow == nullptr)
				continue;
			
			if (pNow->bIsAvailable_Shop)
			{
				CELL_RENDER* pNewRender = new CELL_RENDER;
				pNewRender->pInven = pNow;
				pNewRender->eArray = INVEN_ARRAY_TYPE(i);
				pNewRender->iInven_Index = j;
				m_vecSell_RenderInfo.push_back(pNewRender);
				__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Static))->fRatio = (m_vecSell_RenderInfo.size() % 4) * 0.333f;

				if (((m_vecSell_RenderInfo.size() / 4) > iArraynum) && ((m_vecSell_RenderInfo.size() % 4) > 0))
				{
					++iArraynum;
					fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fSize.y;
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fPosition.y = fStartY + fAdjustY;
				}

				__super::UpdatePart_ByIndex(__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::SHOP_Cell_Static)), 1.f);
				pNewRender->vPos = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Static))->fPosition;
			}
		}
	}

	fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fSize.y * 0.5f;

	if (m_Title_Chest == nullptr)
	{
		m_Title_Chest = new CELL_RENDER;
		m_Title_Chest->bIsTitle = true;
		m_Title_Chest->strTitle = TEXT("보관함");

		fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Title_Text))->fSize.y * 0.5f;
		m_Title_Chest->vPos = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Title_Text))->fPosition;
		m_Title_Chest->vPos.y = fStartY + fAdjustY;
		fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Title_Text))->fSize.y * 0.5f;
	}

	// 보관함 관련 내용 필요 






	m_pScroll_Shop->Activate_Scroll(fAdjustY);
}

void CUIPage_Shop::Setting_BuyTab()
{
	for (auto& iter : m_vecBuy_RenderInfo)
		Safe_Delete(iter);

	m_vecBuy_RenderInfo.clear();

	_float fStartY = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Data_Area))->fPosition.y - __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Data_Area))->fSize.y * 0.5f;
	_float fAdjustY = 0.f;

	fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fSize.y * 0.5f;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fPosition.y = fStartY + fAdjustY;
	_int iArraynum = 0;

	vector<CItem_Manager::SHOP*>& ShopInfo = GET_GAMEINTERFACE->Get_ShopData();

	for (_int i = 0; i > ShopInfo.size(); ++i)
	{
		CELL_RENDER* pNewRender = new CELL_RENDER;
		pNewRender->iIndexShop = i;
		pNewRender->pInven = GET_GAMEINTERFACE->Get_Item_Origin_Spec(ShopInfo[i]->iIndex);
		m_vecBuy_RenderInfo.push_back(pNewRender);
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Static))->fRatio = (m_vecSell_RenderInfo.size() % 4) * 0.333f;
		if (((m_vecBuy_RenderInfo.size() / 4) > iArraynum) && ((m_vecBuy_RenderInfo.size() % 4) > 0))
		{
			++iArraynum;
			fAdjustY += __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fSize.y;
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Array))->fPosition.y = fStartY + fAdjustY;
		}
		__super::UpdatePart_ByIndex(__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::SHOP_Cell_Static)), 1.f);
		pNewRender->vPos = __super::Get_Front_Part_In_Control(_int(PART_GROUP::SHOP_Cell_Static))->fPosition;
	}

	m_pScroll_Shop->Activate_Scroll(fAdjustY);
}

void CUIPage_Shop::Reset_PopupInfo()
{
	m_iNowCell = 0;
	m_iNowCount = 0;
	m_iNowPrice = 0;
	m_bIsItem_Popup = false;
}

CUIPage_Shop* CUIPage_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Shop* pInstance = new CUIPage_Shop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Shop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Shop::Clone(void* pArg)
{
	CUIPage_Shop* pInstance = new CUIPage_Shop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Shop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Shop::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();


	Safe_Delete(m_Title_Bag);
	Safe_Delete(m_Title_Chest);

	for (auto& iter : m_vecSell_RenderInfo)
		Safe_Delete(iter);
	
	m_vecSell_RenderInfo.clear();

	for (auto& iter : m_vecBuy_RenderInfo)
		Safe_Delete(iter);

	m_vecBuy_RenderInfo.clear();
}
