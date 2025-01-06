#include "stdafx.h"
#include "..\Public\UIPage_Inform.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Inform::CUIPage_Inform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Inform::CUIPage_Inform(const CUIPage_Inform& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Inform::Initialize_Prototype()
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

HRESULT CUIPage_Inform::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Inform::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Inform::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Inform::Late_Update(_float fTimeDelta)
{
	if (fTimeDelta > 0.2f)
		fTimeDelta = 0.01f;

	Update_Region(fTimeDelta);
	Update_Inform(fTimeDelta);
	Update_Heart(fTimeDelta);
	Update_PlayerDead_UI(fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Inform::Render()
{
	return S_OK;
}

void CUIPage_Inform::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Inform::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Inform::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Inform::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	for (auto& iter : m_vecPart)
		iter->bRender = false;
	m_bUpdate = true;
	m_bRender = true;
	m_fTopPartMove = -1.f;

	m_fAdjustLength = m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_HourNeedle)]->fAdjust_End.y;
	for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_HourNeedle); i <= _int(PART_GROUP::INFORM_PlayerDeath_ClockInGear); ++i)
		m_vecPart[i]->iMoveType = 0;

	return S_OK;
}

void CUIPage_Inform::Show_Region_Info(_wstring strName, _wstring strDesc, _float fTime_Emerge, _float fTime_Show)
{
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_bUpdate = true;
	m_fTopPartMove = -1.f;
	m_vLifeTime_Region = { 0.f, fTime_Emerge , fTime_Show };

	for (_int i = _int(PART_GROUP::INFORM_Region_Fx); i <= _int(PART_GROUP::INFORM_Region_Desc); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextureColor.w = 0.f;
		m_vecPart[i]->fTextColor.w = 0.f;
	}

	m_vecPart[_int(PART_GROUP::INFORM_Region_Title)]->strText = strName;
	m_vecPart[_int(PART_GROUP::INFORM_Region_Desc)]->strText = strDesc;
}

void CUIPage_Inform::Show_Inform(INFORM_MESSAGE eInform, _float fTime_Emerge, _float fTime_Show)
{
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_bUpdate = true;
	m_fTopPartMove = -1.f;
	m_vLifeTime_Inform = { 0.f, fTime_Emerge , fTime_Show };

	if (eInform == INFORM_MESSAGE::INFORM_STARGAZER)
	{
		m_vecPart[_int(PART_GROUP::INFORM_Stargazer_Fx1)]->bRender = true;
		m_vecPart[_int(PART_GROUP::INFORM_Stargazer_Fx1)]->fTextureColor.w = 0.f;

		m_vecPart[_int(PART_GROUP::INFORM_Stargazer_Fx1)]->fRatio;

		m_vecPart[_int(PART_GROUP::INFORM_Stargazer)]->bRender = true;
		m_vecPart[_int(PART_GROUP::INFORM_Stargazer)]->fTextureColor.w = 0.f;

		GET_GAMEINTERFACE->Input_Achievment_Data(11, 1);
	}
	else
	{
		_int iIndex = _int(PART_GROUP::INFORM_Dead) + _int(eInform);

		for (_int i = _int(PART_GROUP::INFORM_Common_Fx); i <= _int(PART_GROUP::INFORM_BossKill); ++i)
		{
			if ((i == _int(PART_GROUP::INFORM_Common_Fx)) || (i == iIndex))
			{
				m_vecPart[i]->bRender = true;
				m_vecPart[i]->fTextureColor.w = 0.f;
			}
		}

		m_vecPart[_int(PART_GROUP::INFORM_Common_Fx)]->bRender = true;
		m_vecPart[iIndex]->bRender = true;
	}


}

void CUIPage_Inform::Show_Heart(_wstring strScript, _float fTime_Emerge, _float fTime_Show)
{
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_bUpdate = true;
	m_fTopPartMove = -1.f;
	m_vLifeTime_Heart = { 0.f, fTime_Emerge , fTime_Show };

	for (_int i = _int(PART_GROUP::INFORM_Heart); i <= _int(PART_GROUP::INFORM_Text_Front); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextureColor.w = 0.f;
		m_vecPart[i]->fTextColor.w = 0.f;
	}

	m_vecPart[_int(PART_GROUP::INFORM_Text_Back)]->strText = strScript;
	m_vecPart[_int(PART_GROUP::INFORM_Text_Front)]->strText = strScript;
}

void CUIPage_Inform::Show_Dead(_float fTime_Emerge, _float fTime_Show)
{







}

void CUIPage_Inform::Show_PlayerDead_UI()
{
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_bUpdate = true;
	m_fTopPartMove = -1.f;
	m_vPlayerDead_Time = { 0.f, 1.f ,5.f ,1.f };
	m_iPlayerDead_Phase = 0;

	m_fAngle_Clock_Hour = m_fAngle_Clock_Hour_Start;
	m_fAngle_Clock_Minitue = m_fAngle_Clock_Minitue_Start;
	m_fAngle_Clock_Hour_Pos = m_fAngle_Clock_Hour_Start;
	m_fAngle_Clock_Minitue_Pos = m_fAngle_Clock_Minitue_Start;

	m_bIsPlayerDead_UI_NowEnd = false;
	m_bCheckNowEnd = false;

	GET_GAMEINTERFACE->UIPart_Off();

	for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_Back); i <= _int(PART_GROUP::INFORM_PlayerDeath_LieORDie); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextureColor.w = 0.f;
		m_vecPart[i]->fTextColor.w = 0.f;
	}
}

void CUIPage_Inform::Update_Region(_float fTimeDelta)
{
	_float fRatio = Check_Ratio(&m_vLifeTime_Region, fTimeDelta);

	if (fRatio == -1.f)
	{
		for (_int i = _int(PART_GROUP::INFORM_Region_Fx); i <= _int(PART_GROUP::INFORM_Region_Desc); ++i)
			m_vecPart[i]->bRender = false;

	}
	else
	{
		for (_int i = _int(PART_GROUP::INFORM_Region_Fx); i <= _int(PART_GROUP::INFORM_Region_Desc); ++i)
		{
			m_vecPart[i]->fTextureColor.w = fRatio;
			m_vecPart[i]->fTextColor.w = fRatio;
		}
	}
}

void CUIPage_Inform::Update_Inform(_float fTimeDelta)
{
	_float fRatio = Check_Ratio(&m_vLifeTime_Inform, fTimeDelta);

	if (fRatio == -1.f)
	{
		for (_int i = _int(PART_GROUP::INFORM_Common_Fx); i <= _int(PART_GROUP::INFORM_Stargazer); ++i)
			m_vecPart[i]->bRender = false;

		m_vecPart[_int(PART_GROUP::INFORM_Stargazer_Fx1)]->fRatio = 0.f;
	}
	else
	{
		for (_int i = _int(PART_GROUP::INFORM_Common_Fx); i <= _int(PART_GROUP::INFORM_Stargazer); ++i)
		{
			m_vecPart[i]->fTextureColor.w = fRatio;
			m_vecPart[i]->fTextColor.w = fRatio;
		}

		m_vecPart[_int(PART_GROUP::INFORM_Stargazer_Fx1)]->fRatio = m_vLifeTime_Inform.x / (m_vLifeTime_Inform.y * 2.f + m_vLifeTime_Inform.z);
		m_vecPart[_int(PART_GROUP::INFORM_Stargazer_Fx1)]->MovePart(m_vecPart[_int(PART_GROUP::INFORM_Common_Fx)]->fPosition, fTimeDelta);
	}
}

void CUIPage_Inform::Update_Heart(_float fTimeDelta)
{
	if (m_vLifeTime_Heart.z == 0.f)
		return;

	_float fRatio = Check_Ratio(&m_vLifeTime_Heart, fTimeDelta);

	if (fRatio == -1.f)
	{
		for (_int i = _int(PART_GROUP::INFORM_Heart); i <= _int(PART_GROUP::INFORM_Text_Front); ++i)
			m_vecPart[i]->bRender = false;

		GET_GAMEINTERFACE->Exit_Program();
	}
	else
	{
		for (_int i = _int(PART_GROUP::INFORM_Heart); i <= _int(PART_GROUP::INFORM_Text_Front); ++i)
		{
			m_vecPart[i]->fTextureColor.w = fRatio;
			m_vecPart[i]->fTextColor.w = fRatio;
			m_vecPart[i]->fRatio = fRatio;
		}
	}
}

void CUIPage_Inform::Update_Dead(_float fTimeDelta)
{





}

void CUIPage_Inform::Update_PlayerDead_UI(_float fTimeDelta)
{
	if (m_vPlayerDead_Time.x >= 0.f)
		m_vPlayerDead_Time.x += fTimeDelta;
	else 
		return;

	if (m_vPlayerDead_Time.x > (m_vPlayerDead_Time.y + m_vPlayerDead_Time.z + m_vPlayerDead_Time.w))
	{
		m_vPlayerDead_Time = { -1.f,0.f,0.f,0.f };
		for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_Back); i <= _int(PART_GROUP::INFORM_PlayerDeath_LieORDie); ++i)
		{
			m_vecPart[i]->bRender = false;
			m_vecPart[i]->fTextureColor.w = 0.f;
			m_vecPart[i]->fTextColor.w = 0.f;
			if ((i >= _int(PART_GROUP::INFORM_PlayerDeath_HourNeedle)) && (i <= _int(PART_GROUP::INFORM_PlayerDeath_ClockInGear)))
				m_vecPart[i]->fTextureColor = { 1.f,1.f,1.f,0.f };
			m_vecPart[i]->fRatio = 0.f;
		}

		GET_GAMEINTERFACE->UIPart_On();
		return;
	}

	if (m_vPlayerDead_Time.x > (m_vPlayerDead_Time.y + m_vPlayerDead_Time.z))
		m_iPlayerDead_Phase = 2;
	else if (m_vPlayerDead_Time.x > m_vPlayerDead_Time.y)
		m_iPlayerDead_Phase = 1;

	if ((m_iPlayerDead_Phase == 2) && (m_bCheckNowEnd == false))
	{
		m_bCheckNowEnd = true;
		m_bIsPlayerDead_UI_NowEnd = true;
	}
		

	Update_PlayerDead_Back(fTimeDelta, m_iPlayerDead_Phase);
	Update_PlayerDead_Static(fTimeDelta, m_iPlayerDead_Phase);
	Update_PlayerDead_Neddle(fTimeDelta, m_iPlayerDead_Phase);
	Update_PlayerDead_Debris(fTimeDelta, m_iPlayerDead_Phase);
	Update_PlayerDead_Message(fTimeDelta, m_iPlayerDead_Phase);
}

void CUIPage_Inform::Update_PlayerDead_Back(_float fTimeDelta, _int iPhase)
{
	_float fAlpha = 1.f;

	if (iPhase == 0)
		fAlpha = 0.f;
	else if (iPhase == 1)
		fAlpha = ((m_vPlayerDead_Time.x - m_vPlayerDead_Time.y) / m_vPlayerDead_Time.z);
	else if (iPhase == 2)
		fAlpha = (m_vPlayerDead_Time.w - (m_vPlayerDead_Time.x - (m_vPlayerDead_Time.y + m_vPlayerDead_Time.z))) / m_vPlayerDead_Time.w;

	m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_Back)]->fTextureColor.w = fAlpha;
}

void CUIPage_Inform::Update_PlayerDead_Static(_float fTimeDelta, _int iPhase)
{
	_float fAlpha = 1.f;

	if (iPhase == 0)
		fAlpha = m_vPlayerDead_Time.x / m_vPlayerDead_Time.y;
	else if (iPhase == 2)
		fAlpha = (m_vPlayerDead_Time.w - (m_vPlayerDead_Time.x - (m_vPlayerDead_Time.y + m_vPlayerDead_Time.z))) / m_vPlayerDead_Time.w;

	for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_ClockFrame); i <= _int(PART_GROUP::INFORM_PlayerDeath_ClockInnerRing); ++i)
		m_vecPart[i]->fTextureColor.w = fAlpha;
}

void CUIPage_Inform::Update_PlayerDead_Neddle(_float fTimeDelta, _int iPhase)
{
	/*
	1. 각도 구하기 
	2. sin->y, cos->x 좌표 구하기 
	3. x,y에 adjustlength 곱하기 
	4. 위 값을 adjustpos로 사용 
	5. 텍스쳐 각도 만큼 돌리기 
	*/

	if (iPhase == 0)
	{
		m_fAngle_Clock_Hour -= fTimeDelta;
		m_fAngle_Clock_Minitue -= fTimeDelta * 12.f;
		m_fAngle_Clock_Hour_Pos += fTimeDelta;
		m_fAngle_Clock_Minitue_Pos += fTimeDelta * 12.f;
	}
	else 
	{
		_float fAddSpeed = (m_vPlayerDead_Time.x - m_vPlayerDead_Time.y) * 30.f;
		m_fAngle_Clock_Hour -= fTimeDelta * (5.f + fAddSpeed);
		m_fAngle_Clock_Minitue -= fTimeDelta * 12.f * (5.f + fAddSpeed);
		m_fAngle_Clock_Hour_Pos += fTimeDelta * (5.f + fAddSpeed);
		m_fAngle_Clock_Minitue_Pos += fTimeDelta * 12.f * (5.f + fAddSpeed);
	}
		
	m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_HourNeedle)]->fTurn_Degree = m_fAngle_Clock_Hour;
	m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_MinitueNeedle)]->fTurn_Degree = m_fAngle_Clock_Minitue;
	
	m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_HourNeedle)]->fAdjust = { 
		cos(XMConvertToRadians(m_fAngle_Clock_Hour_Pos + m_fAdjust_Clock_Angle)) * m_fAdjustLength,
		sin(XMConvertToRadians(m_fAngle_Clock_Hour_Pos + m_fAdjust_Clock_Angle)) * m_fAdjustLength };
	m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_MinitueNeedle)]->fAdjust = { 
		cos(XMConvertToRadians(m_fAngle_Clock_Minitue_Pos + m_fAdjust_Clock_Angle)) * m_fAdjustLength,
		sin(XMConvertToRadians(m_fAngle_Clock_Minitue_Pos + m_fAdjust_Clock_Angle)) * m_fAdjustLength };

	m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_ClockInGear)]->fTurn_Degree += fTimeDelta;

	_float fAlpha = 1.f;

	if (iPhase == 0)
	{
		fAlpha = m_vPlayerDead_Time.x / m_vPlayerDead_Time.y;
	}
	else if (iPhase == 1)
	{
		if (m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_HourNeedle)]->fTextureColor.x < 2.f)
			for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_HourNeedle); i <= _int(PART_GROUP::INFORM_PlayerDeath_ClockInGear); ++i)
			{
				m_vecPart[i]->fTextureColor.x += fTimeDelta * 5.f;
				m_vecPart[i]->fTextureColor.y -= fTimeDelta * 5.f;
				m_vecPart[i]->fTextureColor.z -= fTimeDelta * 5.f;
				if (m_vecPart[i]->fTextureColor.y < 0.f) m_vecPart[i]->fTextureColor.y = 0.f;
				if (m_vecPart[i]->fTextureColor.z < 0.f) m_vecPart[i]->fTextureColor.z = 0.f;
			}
				
	}
	else if (iPhase == 2)
	{
		fAlpha = (m_vPlayerDead_Time.w - (m_vPlayerDead_Time.x - (m_vPlayerDead_Time.y + m_vPlayerDead_Time.z))) / m_vPlayerDead_Time.w;
	}
		
	for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_HourNeedle); i <= _int(PART_GROUP::INFORM_PlayerDeath_ClockInGear); ++i)
		m_vecPart[i]->fTextureColor.w = fAlpha;
}

void CUIPage_Inform::Update_PlayerDead_Debris(_float fTimeDelta, _int iPhase)
{
	_float fRatio = m_vPlayerDead_Time.x / (m_vPlayerDead_Time.y + (m_vPlayerDead_Time.z * 0.5f));
	fRatio = fRatio > 1.f ? 1.f : fRatio;

	for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_XII); i <= _int(PART_GROUP::INFORM_PlayerDeath_DebrisSmall); ++i)
		m_vecPart[i]->fRatio = fRatio;

	_float fAlpha = 1.f;

	if (iPhase == 0)
		fAlpha = m_vPlayerDead_Time.x / m_vPlayerDead_Time.y;
	else if (iPhase == 2)
		fAlpha = (m_vPlayerDead_Time.w - (m_vPlayerDead_Time.x - (m_vPlayerDead_Time.y + m_vPlayerDead_Time.z))) / m_vPlayerDead_Time.w;
	
	for (_int i = _int(PART_GROUP::INFORM_PlayerDeath_XII); i <= _int(PART_GROUP::INFORM_PlayerDeath_DebrisSmall); ++i)
	{
		m_vecPart[i]->fTextureColor.w = fAlpha;
	}
		
}

void CUIPage_Inform::Update_PlayerDead_Message(_float fTimeDelta, _int iPhase)
{
	_float fAlpha = 1.f;

	if (iPhase == 0)
		fAlpha = m_vPlayerDead_Time.x / m_vPlayerDead_Time.y;
	else if (iPhase == 2)
		fAlpha = (m_vPlayerDead_Time.w - (m_vPlayerDead_Time.x - (m_vPlayerDead_Time.y + m_vPlayerDead_Time.z))) / m_vPlayerDead_Time.w;

	m_vecPart[_int(PART_GROUP::INFORM_PlayerDeath_LieORDie)]->fTextureColor.w = fAlpha;
}

_float CUIPage_Inform::Check_Ratio(_Vec3* vLifeTime, _float fTimeDelta)
{
	_float fResult = 1.f;

	vLifeTime->x += fTimeDelta;

	if (vLifeTime->x >= vLifeTime->y * 2.f + vLifeTime->z)
	{
		fResult = -1.f;
		*vLifeTime = { 0.f,0.f,0.f };
	}
	else if (vLifeTime->x >= vLifeTime->y * 1.f + vLifeTime->z)
	{
		fResult = vLifeTime->x - (vLifeTime->y * 1.f + vLifeTime->z);
		fResult = (vLifeTime->y - fResult) / vLifeTime->y;
	}
	else if (vLifeTime->x >= vLifeTime->y)
	{
		fResult = 1.f;
	}
	else if (vLifeTime->x < vLifeTime->y)
	{
		fResult = vLifeTime->x / vLifeTime->y;
	}


	return fResult;
}

CUIPage_Inform* CUIPage_Inform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Inform* pInstance = new CUIPage_Inform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Inform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Inform::Clone(void* pArg)
{
	CUIPage_Inform* pInstance = new CUIPage_Inform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Inform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Inform::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
