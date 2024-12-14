#include "stdafx.h"
#include "..\Public\FreeCamera.h"

#include "GameInstance.h"

CFreeCamera::CFreeCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera { pDevice, pContext }
{
}

CFreeCamera::CFreeCamera(const CFreeCamera & Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFreeCamera::Initialize(void * pArg)
{
	CAMERA_FREE_DESC*		pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(-27.575f, -81.264f, -14.253f, 1.f));

	return S_OK;
}

void CFreeCamera::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	POINT		ptMouse = m_pGameInstance->Get_MosePos();

	_long	MouseMove = {};

	if (KEY_HOLD(KEY::CTRL))
	{
		/* 기본적인 제어*/
		if (KEY_HOLD(KEY::W))
			m_pTransformCom->Go_Straight(fTimeDelta);
		if (KEY_HOLD(KEY::S))
			m_pTransformCom->Go_Backward(fTimeDelta);
		if (KEY_HOLD(KEY::A))
			m_pTransformCom->Go_Left(fTimeDelta);
		if (KEY_HOLD(KEY::D))
			m_pTransformCom->Go_Right(fTimeDelta);


		if (MouseMove = ptMouse.x - m_ptOldMousePos.x)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
		}

		if (MouseMove = ptMouse.y - m_ptOldMousePos.y)
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
		}
	}

	__super::Priority_Update(fTimeDelta);
	Calculat_CascadeFrustum();

	m_ptOldMousePos = ptMouse;
}

void CFreeCamera::Update(_float fTimeDelta)
{
	Update_Zoom(fTimeDelta);

	if (m_pTransformCom->Get_isTurning())
	{
		Update_Turn(fTimeDelta);
	}

	Update_MoveLerp(fTimeDelta);

	if (m_isShake)
	{
		m_fShakeTime += fTimeDelta;
		if (m_fShakeTime >= m_fShakeDuration)
		{
			m_fShakeTime = 0.f;
			m_isShake = false;
		}

		// 쉐이크 계산 (랜덤 값 생성)
		_float xOffset = (rand() % 100 / 100.0f) * m_fShakePower - (m_fShakePower / 2);
		_float yOffset = (rand() % 100 / 100.0f) * m_fShakePower - (m_fShakePower / 2);
		_vector shakeOffset = XMVectorSet(xOffset, yOffset, 0.f, 0.f);

		// 카메라 위치에 쉐이크 오프셋 추가
		_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		currentPos += shakeOffset;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, currentPos);
	}
}

void CFreeCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CFreeCamera::Render()
{
	return S_OK;
}

CFreeCamera * CFreeCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFreeCamera*		pInstance = new CFreeCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CFreeCamera::Clone(void * pArg)
{
	CFreeCamera*		pInstance = new CFreeCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFreeCamera::Free()
{
	__super::Free();

}
