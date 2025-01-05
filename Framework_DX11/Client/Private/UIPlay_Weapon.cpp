#include "stdafx.h"
#include "..\Public\UIPlay_Weapon.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPlay_Weapon::CUIPlay_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPlay_Component{ pDevice, pContext }
{
}

CUIPlay_Weapon::CUIPlay_Weapon(const CUIPlay_Weapon& Prototype)
	: CUIPlay_Component{ Prototype }
{
}

HRESULT CUIPlay_Weapon::Initialize_Prototype()
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

	m_bIsBatching = false;

	return S_OK;
}

HRESULT CUIPlay_Weapon::Initialize(void* pArg)
{
	return S_OK;
}

void CUIPlay_Weapon::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPlay_Weapon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPlay_Weapon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPlay_Weapon::Render()
{
	return S_OK;
}

void CUIPlay_Weapon::OpenAction()
{
}

void CUIPlay_Weapon::CloseAction()
{
}

CHECK_MOUSE CUIPlay_Weapon::Check_Page_Action(_float fTimeDelta)
{
	return CHECK_MOUSE();
}

void CUIPlay_Weapon::Initialize_Weapon_Component(vector<struct CUIPage::UIPART_INFO*>& vecOrigin)
{
	for (auto& iter : vecOrigin)
	{
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_CENTER))
			m_pSharedPointer_Center = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_DURABLE_FRAME))
			m_vecSharedPointer_DurableGauge_Static.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_DURABLE_FILL))
			m_pSharedPointer_DurableGauge_Fill = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK))
			m_vecSharedPointer_NormalWeapon_Static.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX))
			m_vecSharedPointer_NormalWeapon_Fx.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_NORMAL_BLADE))
			m_pSharedPointer_NormalWeapon_Blade = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_NORMAL_HANDLE))
			m_pSharedPointer_NormalWeapon_Handle = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_SPECIAL_BACK))
			m_vecSharedPointer_SpecialWeapon_Static.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_SPECIAL_TEX))
			m_pSharedPointer_SpecialWeapon = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_EQUIP_NUM))
			m_pSharedPointer_SelectNum = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SYMBOL))
			m_pSharedPointer_BladeFable_Symbol = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SYMBOL))
			m_pSharedPointer_HandleFable_Symbol = iter;
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FRAME))
			m_vecSharedPointer_BladeFable_Side_Frame.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_WHITE))
			m_vecSharedPointer_BladeFable_Side_White.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FILL))
			m_vecSharedPointer_BladeFable_Side_Fill.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FRAME))
			m_vecSharedPointer_BladeFable_Center_Frame.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_WHITE))
			m_vecSharedPointer_BladeFable_Center_White.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FILL))
			m_vecSharedPointer_HandleFable_Side_Fill.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FRAME))
			m_vecSharedPointer_HandleFable_Side_Frame.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_WHITE))
			m_vecSharedPointer_HandleFable_Side_White.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FILL))
			m_vecSharedPointer_HandleFable_Side_Fill.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FRAME))
			m_vecSharedPointer_HandleFable_Center_Frame.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_WHITE))
			m_vecSharedPointer_HandleFable_Center_White.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FILL))
			m_vecSharedPointer_HandleFable_Center_Fill.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_A))
			m_vecSharedPointer_BladeFable_KeySet_F.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_B))
			m_vecSharedPointer_BladeFable_KeySet_Combine.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_A))
			m_vecSharedPointer_HandleFable_KeySet_F.push_back(iter);
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_B))
			m_vecSharedPointer_HandleFable_KeySet_Combine.push_back(iter);
	}


}

void CUIPlay_Weapon::Update_WeaponInfo(_int iFable_Count_Now, _float fTimeDelta)
{

}

void CUIPlay_Weapon::Switch_Weapon()
{
}

void CUIPlay_Weapon::Switch_Mode()
{
}

void CUIPlay_Weapon::Set_Fable()
{
}

void CUIPlay_Weapon::Start_WeaponCell_Change(_float fTimeDelta)
{
}

void CUIPlay_Weapon::Start_FableGauge_Change(_float fTimeDelta)
{
}

void CUIPlay_Weapon::Update_DurableGauge(_float fTimeDelta)
{
}

void CUIPlay_Weapon::Update_WeaponCell_Fx(_float fTimeDelta)
{
}

void CUIPlay_Weapon::Update_FableGauge(_float fTimeDelta)
{
}

CUIPlay_Weapon* CUIPlay_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPlay_Weapon* pInstance = new CUIPlay_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPlay_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPlay_Weapon::Clone(void* pArg)
{
	CUIPlay_Weapon* pInstance = new CUIPlay_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPlay_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPlay_Weapon::Free()
{
	__super::Free();

	m_vecSharedPointer_DurableGauge_Static.clear();
	m_vecSharedPointer_NormalWeapon_Static.clear();
	m_vecSharedPointer_NormalWeapon_Fx.clear();
	m_vecSharedPointer_SpecialWeapon_Static.clear();
	m_vecSharedPointer_BladeFable_Side_Frame.clear();
	m_vecSharedPointer_BladeFable_Side_White.clear();
	m_vecSharedPointer_BladeFable_Side_Fill.clear();
	m_vecSharedPointer_BladeFable_Center_Frame.clear();
	m_vecSharedPointer_BladeFable_Center_White.clear();
	m_vecSharedPointer_BladeFable_Center_Fill.clear();
	m_vecSharedPointer_BladeFable_KeySet_F.clear();
	m_vecSharedPointer_BladeFable_KeySet_Combine.clear();
	m_vecSharedPointer_HandleFable_Side_Frame.clear();
	m_vecSharedPointer_HandleFable_Side_White.clear();
	m_vecSharedPointer_HandleFable_Side_Fill.clear();
	m_vecSharedPointer_HandleFable_Center_Frame.clear();
	m_vecSharedPointer_HandleFable_Center_White.clear();
	m_vecSharedPointer_HandleFable_Center_Fill.clear();
	m_vecSharedPointer_HandleFable_KeySet_F.clear();
	m_vecSharedPointer_HandleFable_KeySet_Combine.clear();
}
