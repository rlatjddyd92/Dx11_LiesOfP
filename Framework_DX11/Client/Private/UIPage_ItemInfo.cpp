#include "stdafx.h"
#include "..\Public\UIPage_ItemInfo.h"

#include "GameInstance.h"


CUIPage_ItemInfo::CUIPage_ItemInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_ItemInfo::CUIPage_ItemInfo(const CUIPage_ItemInfo& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_ItemInfo::Initialize_Prototype()
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

HRESULT CUIPage_ItemInfo::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_ItemInfo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_ItemInfo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_ItemInfo::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CUIPage_ItemInfo::Render()
{
	return S_OK;
}

void CUIPage_ItemInfo::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_ItemInfo::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_ItemInfo::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);
	Action_ItemAction(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_ItemInfo::Ready_UIPart_Group_Control()
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

	__super::Array_Control(_int(PART_GROUP::ITEMINFO_TOOLTIP_Frame), _int(PART_GROUP::ITEMINFO_ACTION_Text_3), CTRL_COMMAND::COM_RENDER, true);

	m_strFuncName[_int(ITEM_FUNC::FUNC_USING)] = TEXT("사용하기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_TO_INVEN)] = TEXT("인벤토리로 이동");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP)] = TEXT("착용하기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_UNEQUIP)] = TEXT("해제하기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_DELETE)] = TEXT("버리기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_TOP)] = TEXT("착용하기 : 위 벨트");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BOTTOM)] = TEXT("착용하기 : 아래 벨트");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BAG)] = TEXT("착용하기 : 보조가방");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_WEAPON_FIRST)] = TEXT("착용하기 : 무기 1번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_WEAPON_SECOND)] = TEXT("착용하기 : 무기 2번");
	Off_ItemAction();

	m_bRender = false;

	return S_OK;
}

void CUIPage_ItemInfo::Show_Focus(_Vec2 vItemCellPos, _Vec2 vItemCellSize)
{
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->fPosition = vItemCellPos;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->fSize = vItemCellSize;

	if (m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->bRender)
	{
		m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_End = vItemCellPos + _Vec2{ -(vItemCellSize.x * 0.4f), vItemCellSize.y * 0.4f };
	}
	else
	{
		m_fFocus_Fire_Move_Ratio = 0.f;
		m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_Start = vItemCellPos + _Vec2{ -(vItemCellSize.x * 0.4f), vItemCellSize.y * 0.4f };
	}

	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->bRender = true;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->bRender = true;
}

void CUIPage_ItemInfo::Off_Focus(_Vec2 vItemCellPos, _Vec2 vItemCellSize)
{
	m_fFocus_Fire_Move_Ratio = 0.f;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->bRender = false;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->bRender = false;
}

void CUIPage_ItemInfo::Show_NewMark(_Vec2 vItemCellPos, _Vec2 vItemCellSize)
{
	_Vec2 vPos = vItemCellPos + (vItemCellSize * 0.5f);
	m_queueNewMarkPos.push(vPos);
}

void CUIPage_ItemInfo::Show_ItemAction(_Vec2 vItemCellPos, _Vec2 vItemCellSize, ITEM_FUNC eFunc0, ITEM_FUNC eFunc1, ITEM_FUNC eFunc2, ITEM_FUNC eFunc3)
{
	//m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Header)].

	m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Back)]->bRender = true;
	m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Deco)]->bRender = true;

	m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Back)]->fRatio = 0.f;



	m_eActive_Func[0] = eFunc0;
	m_eActive_Func[1] = eFunc1;
	m_eActive_Func[2] = eFunc2;
	m_eActive_Func[3] = eFunc3;

	for (_int i = 0; i < 4; ++i)
	{
		if (m_eActive_Func[i] != ITEM_FUNC::FUNC_END)
		{
			m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Back)]->fRatio += 0.25f;
			m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Text_0) + (i * 3)]->strText = m_strFuncName[_int(m_eActive_Func[i])];
		}
		else
			break;
	}








}

void CUIPage_ItemInfo::Off_ItemAction()
{
	__super::Array_Control(_int(PART_GROUP::ITEMINFO_ACTION_Header), _int(PART_GROUP::ITEMINFO_ACTION_Text_3), CTRL_COMMAND::COM_RENDER, true);
	for (_int i = 0; i < 4; ++i)
		m_eActive_Func[i] = ITEM_FUNC::FUNC_END;
}

void CUIPage_ItemInfo::Action_ItemAction(_float fTimeDelta)
{
	for (_int i = 0; i < 4; ++i)
	{
		if (m_eActive_Func[i] != ITEM_FUNC::FUNC_END)
		{


			/*m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Mouse_0) + (i * 3)]
				m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Fx_0) + (i * 3)]
				m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Text_0) + (i * 3)]*/

		}
		else
			break;
	}





}


CUIPage_ItemInfo* CUIPage_ItemInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_ItemInfo* pInstance = new CUIPage_ItemInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_ToolTip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_ItemInfo::Clone(void* pArg)
{
	CUIPage_ItemInfo* pInstance = new CUIPage_ItemInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_ToolTip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_ItemInfo::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}