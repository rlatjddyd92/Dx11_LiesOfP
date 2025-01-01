#include "stdafx.h"

#include "PlayerCamera.h"
#include "GameInstance.h"

#include "Player.h"
#include "Monster.h"
#include "Fsm.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CPlayerCamera::CPlayerCamera(const CCamera& Prototype)
	:CCamera{ Prototype }
{
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
	CAMERA_PLAYER_DESC* pDesc = static_cast<CAMERA_PLAYER_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	m_fSpeed = pDesc->fSpeed;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_Vec3 vInitPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION) + _Vec4(0.0f, 5.0f, -3.0f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vInitPos);

	m_vOffset = _Vec3(0.f, 1.8f, 0.f);

	m_fRadian = m_vOffset.Dot(_Vec3(0.f, 1.f, 0.f));
	m_fRadian = acosf(m_fRadian);

	return S_OK;
}

void CPlayerCamera::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CPlayerCamera::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_isMoveInitPos)
	{
		PlayerInitMove(fTimeDelta);
	}
	else if (m_pPlayer->Get_IsLockOn())
	{
		PlayerLockOn(fTimeDelta);
	}
	else
	{
		PlayerMove(fTimeDelta);
	}

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

	Calculat_CascadeFrustum();
}

void CPlayerCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}

void CPlayerCamera::PlayerMove(_float fTimeDelta)
{
	_long      MouseMove = { 0 };

	_vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	vPlayerPos.m128_f32[1] += 1.65f;

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_X))
	{
		m_pTransformCom->Orbit(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPlayerPos, 0.3f, 2.8f, fTimeDelta * MouseMove * 0.1f);
	}
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_Y))
	{
		m_pTransformCom->Orbit(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), vPlayerPos, 0.8f, 2.8f, fTimeDelta * MouseMove * 0.1f);
	}
	m_vTargetPos = vPlayerPos - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 2.8f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTargetPos);

	m_pTransformCom->LookAt(vPlayerPos);
}

void CPlayerCamera::PlayerLockOn(_float fTimeDelta)
{
	if (nullptr == m_pPlayer->Get_TargetMonster())
		return;

	m_pTransformCom->LookAt(m_pPlayer->Get_TargetMonster()->Calc_CenterPos());

	_long      MouseMove = { 0 };

	_vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	vPlayerPos.m128_f32[1] += 2.f;

	m_vTargetPos = vPlayerPos - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f;

	// Lerp로 카메라 위치를 부드럽게 이동
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vNewPos = XMVectorLerp(vCurrentPos, m_vTargetPos, 0.3f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
}

void CPlayerCamera::PlayerInitMove(_float fTimeDelta)
{
	_vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	vPlayerPos.m128_f32[1] += 1.75f;

	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_Vec3 vNewPos = XMVectorLerp(vCurrentPos, m_vTargetPos, 0.07f); 

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	m_pTransformCom->LookAt(vPlayerPos);

	if ((vNewPos - m_vTargetPos).Length() < 0.05f)
	{
		m_isMoveInitPos = false;
	}
}

void CPlayerCamera::Move_PlayerBackPos()
{
	if (!m_isMoveInitPos)
	{
		m_isMoveInitPos = true;

		_Vec4 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vPlayerPos.y += 1.65f;

		_Vec3 vPlayerLook = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);
		vPlayerLook.y = 0.f;
		vPlayerLook.Normalize();

		_Vec3 vInitPos = vPlayerPos - vPlayerLook * 7.f;
		vInitPos.y += 2.8f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitPos);
		m_pTransformCom->LookAt(vPlayerPos);

		m_vTargetPos = (_Vec3)vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 2.8f;
	}
}

HRESULT CPlayerCamera::Ready_Components()
{
	return S_OK;
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CPlayerCamera::Clone(void* pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();
}
