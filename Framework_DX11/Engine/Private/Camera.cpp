#include "..\Public\Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CCamera::CCamera(const CCamera & Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	CAMERA_DESC*	pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	//XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	m_fFovy = pDesc->fFovy;
	m_fInitFovy = m_fFovy;
	m_fAspect = pDesc->fAspect;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
	m_pGameInstance->Set_NearFar(m_fNear, m_fFar);
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Start_PosShake(_float fPower, _float fDuration)
{
	m_fShakePower = fPower;
	m_fShakeDuration = fDuration;
	m_fShakeTime = 0.f;
	m_vOriginLook = _float3(0.f, 0.f, 0.f);

	m_isShake = true;
}

void CCamera::ZoomIn(_float fFovy, _float fDuration)
{
	if (m_isZoomIn)
		return;

	m_fTargetFovy = fFovy;
	m_fZoomDuration = fDuration;
	m_fZoomTime = 0.f;

	m_isZoomIn = true;
	m_isZoomOut = false;
}

void CCamera::ZoomOut(_float fDuration)
{
	m_fZoomInFovy = m_fFovy;
	m_fTargetFovy = m_fInitFovy;
	m_fZoomDuration = fDuration;
	m_fZoomTime = 0.f;

	m_isZoomIn = false;
	m_isZoomOut = true;
}

void CCamera::Start_Turn(_float fSpeed, _Vec3 vPitchYawRoll)
{
	m_bTurn = true;

	m_fTurnSpeed = fSpeed;
	m_vTarget_PitchTawRoll = vPitchYawRoll;
	m_pTransformCom->Set_isTurning(true);
}

void CCamera::Reset_Zoom()
{
	m_fFovy = m_fInitFovy;
	m_fTargetFovy = m_fInitFovy;
	m_isZoomIn = false;
	m_isZoomOut = false;
	m_fZoomDuration = 0.f;
	m_fZoomTime = 0.f;
}

void CCamera::Reset_MoveLerp()
{
	m_bMoveLerp = false;
	m_fMoveSpeed = 0.f;
}

void CCamera::Start_MoveLerp(_Vec3 vTargetPos, _float fSpeed)
{
	m_bMoveLerp = true;
	m_vTarget_Pos = vTargetPos;
	m_fMoveSpeed = fSpeed;
}

void CCamera::Calculat_CascadeFrustum()
{
	if (!m_pGameInstance->Get_IsOnShadow())
		return;

	_Matrix CascadeViewMatrix[3];
	_Matrix CascadeProjMatrix[3];
	_Matrix CascadeProjInverseMatrix[3];

	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_Matrix ViewMatrix = WorldMatrix.Invert();

	_Vec3 vLightDir = m_pGameInstance->Get_DirectionLightDir();

	_float fTanHalfVFov = tanf(m_fFovy * 0.5f);
	_float fTanHalfHFov = fTanHalfVFov * m_fAspect;

	m_fCascadeFarPlanes[0] = m_fNear;
	m_fCascadeFarPlanes[1] = 15.f;
	m_fCascadeFarPlanes[2] = 30.f;
	m_fCascadeFarPlanes[3] = 300.f;

	for (_uint i = 0; i < 3; ++i)
	{
		float fNearX = m_fCascadeFarPlanes[i] * fTanHalfHFov;
		float fNearY = m_fCascadeFarPlanes[i] * fTanHalfVFov;

		float fFarX = m_fCascadeFarPlanes[i + 1] * fTanHalfHFov;
		float fFarY = m_fCascadeFarPlanes[i + 1] * fTanHalfVFov;

		//Near Far 평면 구성
		_Vec4 vFrustumCorners[8] =
		{
			{fNearX, fNearY, m_fCascadeFarPlanes[i], 1.0f},
			{-fNearX, fNearY, m_fCascadeFarPlanes[i], 1.0f},
			{fNearX, -fNearY, m_fCascadeFarPlanes[i], 1.0f},
			{-fNearX, -fNearY, m_fCascadeFarPlanes[i], 1.0f},

			{fFarX, fFarY, m_fCascadeFarPlanes[i + 1], 1.0f},
			{-fFarX, fFarY, m_fCascadeFarPlanes[i + 1],1.0f},
			{fFarX, -fFarY, m_fCascadeFarPlanes[i + 1],1.0f},
			{-fFarX, -fFarY, m_fCascadeFarPlanes[i + 1],1.0f}
		};

		//절두체 중심점 구하기
		_Vec4 vCenterPos = {};
		for (_uint j = 0; j < 8; ++j)
		{
			vFrustumCorners[j] = XMVector4Transform(vFrustumCorners[j], WorldMatrix);
			vCenterPos += vFrustumCorners[j];
		}

		vCenterPos /= 8.f;

		_float fRadius = 0.f;
		for (_uint j = 0; j < 8; ++j) 
		{
			_float fDistance = (vFrustumCorners[j] - vCenterPos).Length();
			fRadius = max(fDistance, fRadius);
		}
		fRadius = ceil(fRadius * 16.f) / 16.f;

		_Vec3 vMaxExtents = _Vec3(fRadius, fRadius, fRadius + 60.f);
		_Vec3 vMinExtents = -vMaxExtents;

		// 움직였음?
		_Vec3 vShadowCamPos = _Vec3(vCenterPos) + (vLightDir * vMinExtents.z);
		//if (fabs((vShadowCamPos - m_vPrevCenterPos[i]).Length()) < 5.f) 
		//{
		//	return;
		//}
		m_vPrevCenterPos[i] = vShadowCamPos;

		// 위치, look, up
		_Matrix LightMatrix = _Matrix::CreateWorld(vShadowCamPos, -vLightDir, _float3(0.0f, 1.0f, 0.0f));
		CascadeViewMatrix[i] = LightMatrix.Invert();

		_Vec3 vCascadeExtents = vMaxExtents - vMinExtents;

		CascadeProjMatrix[i] = XMMatrixOrthographicLH(vCascadeExtents.x, vCascadeExtents.y, 0.1f, vCascadeExtents.z);
		CascadeProjInverseMatrix[i] = XMMatrixOrthographicLH(vCascadeExtents.x, vCascadeExtents.y, vCascadeExtents.z, 0.f);
	}

	m_pGameInstance->Set_CascadeViewMatirx(CascadeViewMatrix);
	m_pGameInstance->Set_CascadeProjMatirx(CascadeProjMatrix);
	m_pGameInstance->Set_CascadeProjInverseMatirx(CascadeProjInverseMatrix);
}

void CCamera::Update_Zoom(_float fTimeDelta)
{
	if (m_isZoomIn)
	{
		m_fZoomTime += fTimeDelta;
		if (m_fZoomTime >= m_fZoomDuration)
		{
			m_fFovy = m_fTargetFovy;
			m_fZoomTime = m_fZoomDuration;
		}
		else
			m_fFovy = m_pGameInstance->Lerp(m_fInitFovy, m_fTargetFovy, m_fZoomTime / m_fZoomDuration);
	}
	else if (m_isZoomOut)
	{
		m_fZoomTime += fTimeDelta;
		if (m_fZoomTime >= m_fZoomDuration)
		{
			m_fFovy = m_fTargetFovy;
			m_fZoomTime = m_fZoomDuration;
		}
		else
		{
			m_fFovy = m_pGameInstance->Lerp(m_fZoomInFovy, m_fTargetFovy, m_fZoomTime / m_fZoomDuration);
		}
	}
}

void CCamera::Update_Turn(_float fTimeDelta)
{
	if (m_bTurn == false)
		return;

	m_pTransformCom->Turn_RollPitchYaw_Lerp(m_vTarget_PitchTawRoll.x, m_vTarget_PitchTawRoll.y, m_vTarget_PitchTawRoll.z, m_fTurnSpeed, fTimeDelta);
}

void CCamera::Update_MoveLerp(_float fTimeDelta)
{
	if (m_bMoveLerp == false)
		return;

	_Vec4 vTarget = { m_vTarget_Pos.x,m_vTarget_Pos.y,m_vTarget_Pos.z, 1.f };
	_Vec4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (fabs(XMVectorGetX(vCurPos - vTarget)) <= 0.005f &&
		fabs(XMVectorGetY(vCurPos - vTarget)) <= 0.005f &&
		fabs(XMVectorGetZ(vCurPos - vTarget)) <= 0.005f
		)
	{
		// 회전 완료 처리
		m_bMoveLerp = false;
	}
	m_pTransformCom->Go_Lerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vTarget, m_fMoveSpeed);
}

void CCamera::Free()
{
	__super::Free();

}
