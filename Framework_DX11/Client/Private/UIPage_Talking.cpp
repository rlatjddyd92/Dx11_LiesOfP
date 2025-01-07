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

	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

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

	Update_Script(fTimeDelta);

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

		if (iter[6][0] != 'x')
			pNewScript->strFunction_A = iter[6];
		if (iter[7][0] != 'x')
			pNewScript->strFunction_B = iter[7];
		if (iter[8][0] != 'x')
			pNewScript->strFunction_C = iter[8];

		if (iter[9][0] != 'x')
			pNewScript->strSound = iter[9];

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
}

void CUIPage_Talking::Show_Select_Script(_wstring strLeft, _wstring strRight, _float fTime)
{
	


}

_bool CUIPage_Talking::IsTalking_WithNPC(NPC_SCRIPT eNPC)
{
	if (IsTalking_WithNPC() == false)
		return false;

	if (m_eNowNpc != eNPC)
		return false;

	if (IsEndTalk_WithNPC(eNPC) == true)
		return false;

	return true;
}

_bool CUIPage_Talking::IsEndTalk_WithNPC(NPC_SCRIPT eNPC)
{
	if (m_vecNpc_ScriptInfo[_int(eNPC)]->bIsEnded == true)
		return true;

	return false;
}

void CUIPage_Talking::Update_Script(_float fTimeDelta)
{
	if (m_eNowNpc == NPC_SCRIPT::SCR_END)
		return;

	for (_int i = _int(PART_GROUP::TALKING_Function_Top_Center); i <= _int(PART_GROUP::TALKING_Function_Rect_Right); ++i)
		m_vecPart[i]->bRender = false;

	SCRIPT* pNow = m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->Get_NowScript();

	if (pNow->strFunction_A.empty())
	{
		if (KEY_TAP(KEY::LBUTTON))
			if (m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->Add_ScriptNum() == false)
			{
				CloseAction();

				m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->bIsEnded = true;

				if (m_eNowNpc == NPC_SCRIPT::SCR_ZEMINI)
				{
					GET_GAMEINTERFACE->Start_Tutorial();
					GET_GAMEINTERFACE->UIPart_On();
				}
				else if (m_eNowNpc == NPC_SCRIPT::SCR_ZEMINI_SECOND)
				{
					GET_GAMEINTERFACE->Show_Heart(TEXT("심장이 고동친다."));
				}
				else
				{
					GET_GAMEINTERFACE->UIPart_On();
				}
					
				return;
			}
	}
	else
	{
		_bool bIsClick = KEY_TAP(KEY::LBUTTON);
		m_iFunction = -1;

		if (!pNow->strFunction_A.empty())
		{
			m_vecPart[_int(PART_GROUP::TALKING_Function_Back_0)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Fx_0)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Text_0)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Text_0)]->strText = pNow->strFunction_A;

			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::TALKING_Function_Back_0)]->fPosition, m_vecPart[_int(PART_GROUP::TALKING_Function_Back_0)]->fSize).x != -1.f)
			{
				m_vecPart[_int(PART_GROUP::TALKING_Function_Rect_Left)]->iParentPart_Index = _int(PART_GROUP::TALKING_Function_Back_0);
				m_iFunction = 0;
			}
		}
		if (!pNow->strFunction_B.empty())
		{
			m_vecPart[_int(PART_GROUP::TALKING_Function_Back_1)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Fx_1)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Text_1)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Text_1)]->strText = pNow->strFunction_B;

			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::TALKING_Function_Back_1)]->fPosition, m_vecPart[_int(PART_GROUP::TALKING_Function_Back_1)]->fSize).x != -1.f)
			{
				m_vecPart[_int(PART_GROUP::TALKING_Function_Rect_Left)]->iParentPart_Index = _int(PART_GROUP::TALKING_Function_Back_1);
				m_iFunction = 1;
			}
		}
		if (!pNow->strFunction_C.empty())
		{
			m_vecPart[_int(PART_GROUP::TALKING_Function_Back_2)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Fx_2)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Text_2)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Text_2)]->strText = pNow->strFunction_C;

			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::TALKING_Function_Back_2)]->fPosition, m_vecPart[_int(PART_GROUP::TALKING_Function_Back_2)]->fSize).x != -1.f)
			{
				m_vecPart[_int(PART_GROUP::TALKING_Function_Rect_Left)]->iParentPart_Index = _int(PART_GROUP::TALKING_Function_Back_2);
				m_iFunction = 2;
			}
		}
		
		if (m_iFunction != -1)
		{
			m_vecPart[_int(PART_GROUP::TALKING_Function_Rect_Left)]->iParentPart_Index = _int(PART_GROUP::TALKING_Function_Back_0) + m_iFunction;

			m_vecPart[_int(PART_GROUP::TALKING_Function_Rect_Left)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Rect_Middle)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TALKING_Function_Rect_Right)]->bRender = true;
		}
		
		
		if (bIsClick)
		{
			if (m_iFunction == -1)
			{
				if (m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->Add_ScriptNum() == false)
				{
					GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_TALKING, UIPAGE::PAGE_PLAY);
					return;
				}
			}
			else if (m_iFunction == 0)
			{
				GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_TALKING, UIPAGE::PAGE_TELEPOT);
			}
			else if (m_iFunction == 1)
			{
				GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_TALKING, UIPAGE::PAGE_SHOP);
			}
			else if (m_iFunction == 2)
			{
				GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_TALKING, UIPAGE::PAGE_CHEST);
			}
		}
	}

	if (pNow->strSpeaker[0] == 'S')
	{
		Update_Select(fTimeDelta);

		m_vecPart[_int(PART_GROUP::TALKING_Name_Text)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Script_0)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Script_1)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Script_2)]->bRender = false;
	}
	else
	{
		for (_int i = _int(PART_GROUP::TALKING_Select_Clock_Center); i <= _int(PART_GROUP::TALKING_Select_Right_Text); ++i)
			m_vecPart[i]->bRender = false;

		m_vSelectTime.x = -1.f;

		m_vecPart[_int(PART_GROUP::TALKING_Name_Text)]->strText = pNow->strSpeaker;
		m_vecPart[_int(PART_GROUP::TALKING_Script_0)]->strText = pNow->strScript_A;
		m_vecPart[_int(PART_GROUP::TALKING_Script_1)]->strText = pNow->strScript_B;
		m_vecPart[_int(PART_GROUP::TALKING_Script_2)]->strText = pNow->strScript_C;

		m_vecPart[_int(PART_GROUP::TALKING_Name_Text)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Script_0)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Script_1)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Script_2)]->bRender = true;
	}

	m_vecPart[_int(PART_GROUP::TALKING_Back_Fx)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Line)]->bRender = true;
	
	if ((pNow->bSoundStart == true) && (!pNow->strSound.empty()))
	{
		pNow->bSoundStart = false;

		_int iSize = (_int)pNow->strSound.size();

		TCHAR* szFileName = new TCHAR[iSize];

		for (_int i = 0; i < iSize; ++i)
		{
			szFileName[i] = pNow->strSound[i];
		}

		m_pSoundCom->Play2D(szFileName, &g_fCutSceneVolume);

		Safe_Delete_Array(szFileName);
	}
}

void CUIPage_Talking::Update_Select(_float fTimeDelta)
{
	// 중간 발표 이후로 진행
	if (m_vSelectTime.x == -1.f)
		m_vSelectTime.x = m_vSelectTime.y;

	m_vSelectTime.x -= fTimeDelta;
	if (m_vSelectTime.x < 0.f)
		m_vSelectTime.x = 0.f;

	for (_int i = _int(PART_GROUP::TALKING_Select_Clock_Center); i <= _int(PART_GROUP::TALKING_Select_Clock_Indicator); ++i)
		m_vecPart[i]->bRender = true;

	SCRIPT* pNow = m_vecNpc_ScriptInfo[_int(m_eNowNpc)]->Get_NowScript();

	_Vec2 vAngle = { -89.9f, -90.f };

	_float fRemain = (m_vSelectTime.x / m_vSelectTime.y) * 360.f;

	for (_int i = _int(PART_GROUP::TALKING_Select_Clock_Gear_0); i <= _int(PART_GROUP::TALKING_Select_Clock_Indicator); ++i)
	{
		m_vecPart[i]->bTurn = true;
		m_vecPart[i]->fTurn_Degree = fRemain;
	}

	if (fRemain >= 270.f)
	{
		fRemain -= 270.f;
		fRemain *= -1.f;
	}
	else if (fRemain >= 90.f)
	{
		fRemain -= 90.f;
		fRemain = (180.f - fRemain);
	}
	else
	{
		fRemain = -90.f - fRemain;
	}

	vAngle = { fRemain ,-90.f };

	m_vecPart[_int(PART_GROUP::TALKING_Select_Clock_Progress)]->vTexture_Angle = vAngle;

	vAngle = { 0.f,0.f };

	m_vecPart[_int(PART_GROUP::TALKING_Select_Left_Back)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Select_Right_Back)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Select_Left_Text)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Select_Right_Text)]->bRender = true;
	m_vecPart[_int(PART_GROUP::TALKING_Select_Left_Text)]->strText = pNow->strScript_A;
	m_vecPart[_int(PART_GROUP::TALKING_Select_Right_Text)]->strText = pNow->strScript_B;

	if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::TALKING_Select_Left_Back)]->fPosition, m_vecPart[_int(PART_GROUP::TALKING_Select_Left_Back)]->fSize).x != -1.f)
	{
		m_vecPart[_int(PART_GROUP::TALKING_Select_Left_L)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Left_M)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Left_R)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Right_L)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Right_M)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Right_R)]->bRender = false;
	}
	else if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::TALKING_Select_Right_Back)]->fPosition, m_vecPart[_int(PART_GROUP::TALKING_Select_Right_Back)]->fSize).x != -1.f)
	{
		m_vecPart[_int(PART_GROUP::TALKING_Select_Left_L)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Left_M)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Left_R)]->bRender = false;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Right_L)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Right_M)]->bRender = true;
		m_vecPart[_int(PART_GROUP::TALKING_Select_Right_R)]->bRender = true;
	}

	if (KEY_TAP(KEY::LBUTTON))
	{
		for (_int i = _int(PART_GROUP::TALKING_Select_Clock_Center); i <= _int(PART_GROUP::TALKING_Select_Right_Text); ++i)
			m_vecPart[i]->bRender = false;

		if (m_eNowNpc == NPC_SCRIPT::SCR_MANUS)
			GET_GAMEINTERFACE->Input_Achievment_Data(7, 1);
	}


	POINT			ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (_float(ptMouse.x) < 640.f)
	{
		vAngle = { 90.f,-90.f };
	}
	else
	{
		vAngle = { -90.f,90.f };
	}

	m_vecPart[_int(PART_GROUP::TALKING_Select_Clock_Fx)]->vTexture_Angle = vAngle;
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
