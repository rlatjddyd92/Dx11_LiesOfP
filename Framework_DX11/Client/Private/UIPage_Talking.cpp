#include "stdafx.h"
#include "..\Public\UIPage_Talking.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Talking::CUIPage_Talking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Talking::CUIPage_Talking(const CUIPage_Talking& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Talking::Initialize_Prototype()
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

HRESULT CUIPage_Talking::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Talking::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Talking::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_bUpdate = true;
}

void CUIPage_Talking::Late_Update(_float fTimeDelta)
{
	if (m_eNowNpc == NPC_SCRIPT::SCR_END)
		return;

	Update_Script();

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Talking::Render()
{
	return S_OK;
}

void CUIPage_Talking::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Talking::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Talking::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Talking::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	for (auto& iter : m_vecPart)
	{
		iter->bRender = false;
		iter->fTextColor.w = 1.f;
	}
		

	vector<vector<_wstring>> vecBuffer_Spec;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/NPC_Script.csv", &vecBuffer_Spec)))
		return E_FAIL;

	_int iIndex = -1;
	_int iStartNum = 1;

	for (auto& iter : vecBuffer_Spec)
	{
		--iStartNum;
		if (iStartNum >= 0)
			continue;

		if (iIndex < stoi(iter[0]))
		{
			NPC* pNewNpc = new NPC;
			pNewNpc->strName = iter[1];
			++iIndex;
			m_vecNpc_ScriptInfo.push_back(pNewNpc);
		}

		SCRIPT* pNewScript = new SCRIPT;
		pNewScript->strSpeaker = iter[2];
		pNewScript->strScript_A = iter[3];
		if (iter[4][0] != 'x')
		pNewScript->strScript_B = iter[4];
		if (iter[5][0] != 'x')
		pNewScript->strScript_C = iter[5];

		m_vecNpc_ScriptInfo.back()->vecScript.push_back(pNewScript);
	}
	m_bRender = false;
	m_bUpdate = true;
	return S_OK;
}

void CUIPage_Talking::Show_Script(NPC_SCRIPT eNPC, _int iScriptNum)
{
	m_eNowNpc = eNPC;
	m_fTopPartMove = 0.f;
	OpenAction();

	if (iScriptNum != -1)
		m_vecNpc_ScriptInfo[_int(eNPC)]->Set_ScriptNum(iScriptNum);
}

void CUIPage_Talking::Next_Script()
{
	if (m_eNowNpc == NPC_SCRIPT::SCR_END)
		return;

	m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->Add_ScriptNum();
}

void CUIPage_Talking::OFF_Script()
{
	CloseAction();
	GET_GAMEINTERFACE->UIPart_On();
}

void CUIPage_Talking::Show_Select_Script(_wstring strLeft, _wstring strRight, _float fTime)
{
	// 중간 발표 이후로 진행



}

void CUIPage_Talking::Update_Script()
{
	if (m_eNowNpc == NPC_SCRIPT::SCR_END)
		return;

	if (KEY_TAP(KEY::LBUTTON))
		if (m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->Add_ScriptNum() == false)
		{
			CloseAction();
			return;
		}

	SCRIPT* pNow = m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->Get_NowScript();

	m_vecPart[_int(PART_GROUP::TALKING_Name_Text)]->strText = pNow->strSpeaker;
	m_vecPart[_int(PART_GROUP::TALKING_Script_0)]->strText = pNow->strScript_A;
	m_vecPart[_int(PART_GROUP::TALKING_Script_1)]->strText = pNow->strScript_B;
	m_vecPart[_int(PART_GROUP::TALKING_Script_2)]->strText = pNow->strScript_C;
	
	m_vecPart[_int(PART_GROUP::TALKING_Back_Fx)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Name_Text)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Line)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Script_0)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Script_1)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Script_2)]->bRender = true;
}

void CUIPage_Talking::Update_Select()
{
	// 중간 발표 이후로 진행


}

CUIPage_Talking* CUIPage_Talking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Talking* pInstance = new CUIPage_Talking(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Talking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Talking::Clone(void* pArg)
{
	CUIPage_Talking* pInstance = new CUIPage_Talking(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Talking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Talking::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}
	m_vecPart.clear();

	for (auto& iter : m_vecNpc_ScriptInfo)
	{
		Safe_Delete(iter);
	}
	m_vecNpc_ScriptInfo.clear();
	
}
