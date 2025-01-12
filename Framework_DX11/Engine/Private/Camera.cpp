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

	// 캐스캐이드용
	m_fNearFarPlanes[1] = 7.f;
	m_fNearFarPlanes[2] = 25.f;
	m_fNearFarPlanes[3] = 200.f;

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

void CCamera::Calculate_CascadeFrustum()
{
	if (!m_pGameInstance->Get_IsOnShadow())
		return;

	m_fNearFarPlanes[0] = m_fNear;

	// 3단계로 구성
	_Matrix CascadeViewMatrices[3];
	_Matrix CascadeProjMatrices[3];

	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_Matrix ViewMatrix = WorldMatrix.Invert();

	_Vec3 vLightDir = m_pGameInstance->Get_DirectionLightDir();	// 방향성 광원의 방향 받아오기

	// 수직, 수평 절반의 탄젠트
	// 평면의 크기를 구하기 위한 용도
	_float fVerticalFovHalfTangent = tanf(m_fFovy * 0.5f);
	_float fHorizontalFovHalfTangent = fVerticalFovHalfTangent * m_fAspect;

	for (_uint i = 0; i < 3; ++i)
	{
		_float fNearX = m_fNearFarPlanes[i] * fVerticalFovHalfTangent;
		_float fNearY = m_fNearFarPlanes[i] * fVerticalFovHalfTangent;

		_float fFarX = m_fNearFarPlanes[i + 1] * fVerticalFovHalfTangent;
		_float fFarY = m_fNearFarPlanes[i + 1] * fVerticalFovHalfTangent;

		//Near Far 평면 구성
		_Vec4 vFrustumCorners[8] =
		{
			{-fNearX, fNearY, m_fNearFarPlanes[i], 1.0f},	// 좌상
			{fNearX, fNearY, m_fNearFarPlanes[i], 1.0f},	// 우상
			{fNearX, -fNearY, m_fNearFarPlanes[i], 1.0f},	// 우하
			{-fNearX, -fNearY, m_fNearFarPlanes[i], 1.0f},	// 좌하

			{-fFarX, fFarY, m_fNearFarPlanes[i + 1],1.0f},	// 좌상
			{fFarX, fFarY, m_fNearFarPlanes[i + 1], 1.0f},	// 우상
			{fFarX, -fFarY, m_fNearFarPlanes[i + 1],1.0f},	// 우하
			{-fFarX, -fFarY, m_fNearFarPlanes[i + 1],1.0f}	// 좌하
		};

		//절두체 중심점 구하기
		_Vec4 vCenterPos = {};
		for (_uint j = 0; j < 8; ++j)
		{
			vFrustumCorners[j] = XMVector4Transform(vFrustumCorners[j], WorldMatrix);
			vCenterPos += vFrustumCorners[j];
		}
		vCenterPos /= 8.f;

		// 가장 먼 모서리까지의 거리
		_float fRadius = 0.f;
		for (_uint j = 0; j < 8; ++j) 
		{
			_float fDistance = (vFrustumCorners[j] - vCenterPos).Length();
			fRadius = max(fDistance, fRadius);
		}
		fRadius = ceil(fRadius * 16.f) / 16.f;	// 올림

		_Vec3 vMaxExtents = _Vec3(fRadius, fRadius, fRadius + 5.f);
		_Vec3 vMinExtents = -vMaxExtents;

		_Vec3 vShadowCamPos = _Vec3(vCenterPos) + (vLightDir * vMinExtents.z);

		// 위치, look, up
		_Matrix LightMatrix = _Matrix::CreateWorld(vShadowCamPos, -vLightDir, _float3(0.0f, 1.0f, 0.0f));
		CascadeViewMatrices[i] = LightMatrix.Invert();

		_Vec3 vCascadeExtents = vMaxExtents - vMinExtents;

		CascadeProjMatrices[i] = XMMatrixOrthographicLH(vCascadeExtents.x, vCascadeExtents.y, 0.1f, vCascadeExtents.z);
	}

	m_pGameInstance->Set_CascadeViewMatrices(CascadeViewMatrices);
	m_pGameInstance->Set_CascadeProjMatrices(CascadeProjMatrices);
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

	if (fabs(XMVectorGetX(vCurPos - vTarget)) <= 0.01f &&
		fabs(XMVectorGetY(vCurPos - vTarget)) <= 0.01f &&
		fabs(XMVectorGetZ(vCurPos - vTarget)) <= 0.01f
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
