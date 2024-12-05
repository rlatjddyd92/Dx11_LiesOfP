#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "GameInstance.h"
#include "Camera.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	m_WorldMatrix = XMMatrixIdentity();

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC*	pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
	
}

_Vec3 CTransform::Get_Scaled() const
{
	_Matrix		WorldMatrix = m_WorldMatrix;

	return _float3(WorldMatrix.Right().Length(), WorldMatrix.Up().Length(), WorldMatrix.Forward().Length());
}

void CTransform::Set_Scaled(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

void CTransform::LookAt(_Vec4 vAt)
{
	_Vec3			vScale = Get_Scaled();

	_Vec4		vPosition = Get_State(STATE_POSITION);

	_Vec4		vLook = vAt - vPosition;

	_Vec4		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_Vec4		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_NoHeight(_Vec4 vAt)
{
	_Vec3		vScale = Get_Scaled();

	_Vec3	vPosition = Get_State(STATE_POSITION);
	_Vec3	vUp = { 0.f,1.f,0.f };

	_Vec3		vLook = vAt - vPosition;

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_Vec3		vRight = vUp.Cross(vLook);
	
	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Lerp(_Vec4 vLook, _float fSpeed, _float fTimeDelta)
{
	/* 팀원들에게 보여주시 위한 예시 */
	_Vec3 vPlayerLook = Get_State(STATE_LOOK);
	vPlayerLook.Normalize();

	_Vec3 vTargetLook = vLook;
	vTargetLook.Normalize();

	_Vec3 vUp = Get_State(STATE_UP);
	vUp.Normalize();

	_Vec3 vTargetRight = vUp.Cross(vTargetLook);
	vTargetLook = vTargetRight.Cross(vUp);
	vTargetLook.Normalize();

	// 1에 가까울 수록 같은 방향을 의미함
	_float fRadian = acosf(min(1.f, vTargetLook.Dot(vPlayerLook)));

	// 목표에 도달함
	if (fRadian <= fSpeed * fTimeDelta)
	{
		vPlayerLook = vTargetLook;

		_Vec3 vRight = vUp.Cross(vPlayerLook);
		vRight.Normalize();

		vPlayerLook = vRight.Cross(vUp);
		vPlayerLook.Normalize();

		vUp = vPlayerLook.Cross(vRight);
		vUp.Normalize();

		_Vec3 vScale = Get_Scaled();

		vRight *= vScale.x;
		vUp *= vScale.y;
		vPlayerLook *= vScale.z;

		Set_State(STATE::STATE_RIGHT, vRight);
		Set_State(STATE::STATE_UP, vUp);
		Set_State(STATE::STATE_LOOK, vPlayerLook);
	}
	else
	{
		_Vec3 vPlayerUp = Get_State(STATE_UP);
		_Vec3 vCrossUp = vPlayerLook.Cross(vTargetLook);

		if (vPlayerUp.Dot(vCrossUp) >= 0)
		{
			Turn((_Vec4)vUp, fTimeDelta, fSpeed);
		}
		else
		{
			Turn((_Vec4)vUp, fTimeDelta, fSpeed);
		}
	}
}

_int CTransform::LookAt_Lerp_NoHeight(_Vec4 vLook, _float fSpeed, _float fTimeDelta)
{	
	/* 팀원들에게 보여주시 위한 예시2 */
	_Vec3 vPlayerLook = Get_State(STATE_LOOK);
	vPlayerLook.y = 0.f;
	vPlayerLook.Normalize();

	_Vec3 vTargetLook = vLook;
	vTargetLook.y = 0.f;
	vTargetLook.Normalize();

	_Vec3 vUp = Get_State(STATE_UP);
	vUp.Normalize();

	_Vec3 vTargetRight = vUp.Cross(vTargetLook);
	vTargetLook = vTargetRight.Cross(vUp);
	vTargetLook.Normalize();

	// 1에 가까울 수록 같은 방향을 의미함
	_float fRadian = acosf(min(1.f, vTargetLook.Dot(vPlayerLook)));
	_Vec3 vWorldUp = _Vec3(0.0f, 1.0f, 0.0f);

	// 목표에 도달함
	if (fRadian <= fSpeed * fTimeDelta)
	{
		vPlayerLook = vTargetLook;

		_Vec3 vRight = vWorldUp.Cross(vPlayerLook);
		vRight.Normalize();

		vPlayerLook = vRight.Cross(vUp);
		vPlayerLook.Normalize();

		vUp = vPlayerLook.Cross(vRight);
		vUp.Normalize();

		_Vec3 vScale = Get_Scaled();

		vRight *= vScale.x;
		vUp *= vScale.y;
		vPlayerLook *= vScale.z;

		Set_State(STATE::STATE_RIGHT, vRight);
		Set_State(STATE::STATE_UP, vUp);
		Set_State(STATE::STATE_LOOK, vPlayerLook);

		return 0;
	}
	else
	{
		_Vec3 vPlayerUp = Get_State(STATE_UP);
		_Vec3 vCrossUp = vPlayerLook.Cross(vTargetLook);

		if (vPlayerUp.Dot(vCrossUp) >= 0)
		{
			Turn((_Vec4)vWorldUp, fTimeDelta, fSpeed);

			return 1;
		}
		else
		{
			Turn((_Vec4)vWorldUp, fTimeDelta, -fSpeed);

			return -1;
		}
	}
}

void CTransform::LookAt_Dir(_Vec4 vDir)
{
	_Vec3		vScale = Get_Scaled();

	_Vec3	vUp = Get_State(STATE::STATE_UP);
	_Vec3	vLook = vDir;
	_Vec3	vRight = vUp.Cross(vLook);

	vLook = vRight.Cross(vUp);

	vUp.Normalize();
	vLook.Normalize();
	vRight.Normalize();

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_UP, vUp * vScale.y);
	Set_State(STATE_LOOK, vLook * vScale.z);
}

void CTransform::Look_Dir(_Vec4 vDir)
{
	_Vec3		vScale = Get_Scaled();

	_Vec4		vPosition = Get_State(STATE_POSITION);

	_Vec4		vLook = vDir;

	_Vec4		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_Vec4		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);

}

void CTransform::Go_Straight(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation)
{
	_Vec3		vPosition = Get_State(STATE_POSITION);
	_Vec3		vLook = Get_State(STATE_LOOK);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition -= XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition -= XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Lerp(_Vec4 _CurrentPos, _Vec4 _TargetPos, _float _fSpeed, class CNavigation* pNavigation)
{
	_vector vPosition = _CurrentPos;

	_vector vDir = XMVector3Normalize(_TargetPos - _CurrentPos);
	vPosition += vDir * _fSpeed;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);

}

void CTransform::Turn(_Vec4 vAxis, _float fTimeDelta, _float fSpeed)
{
	if (fSpeed == 0.f)
		fSpeed = m_fRotationPerSec;

	_Quaternion vQuaternion = Quaternion::CreateFromAxisAngle((_Vec3)vAxis, fSpeed * fTimeDelta);
	_Matrix matRot = Matrix::CreateFromQuaternion(vQuaternion);

	_Matrix WorldMatrix = m_WorldMatrix * matRot;

	Set_State(STATE_RIGHT, _Vec3(WorldMatrix.m[0]));
	Set_State(STATE_UP, _Vec3(WorldMatrix.m[1]));
	Set_State(STATE_LOOK, _Vec3(WorldMatrix.m[2]));
}

void CTransform::Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta, _float fSpeed)
{
	if (fSpeed == 0.f)
		fSpeed = m_fRotationPerSec;

	_Vec4		vRotation = XMQuaternionRotationRollPitchYaw(fSpeed * fTimeDelta * isRotationX,
		fSpeed * fTimeDelta * isRotationY,
		fSpeed * fTimeDelta * isRotationZ);

	_Vec4		vRight = Get_State(STATE_RIGHT);
	_Vec4		vUp = Get_State(STATE_UP);
	_Vec4		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn_Lerp(_fvector vDir, _float fTurnSpeed, _float fTimeDelta)
{
	if (XMVectorGetY(vDir) == -1.f)
		return;

	_vector vCurrentLook = Get_State(STATE::STATE_LOOK);
	vCurrentLook = XMVector3Normalize(vCurrentLook);

	if (XMVectorGetX(XMVector3Length(vCurrentLook - vDir)) < 0.1f)
		return;

	_vector vCurrentUp = Get_State(STATE::STATE_UP);
	vCurrentUp = XMVector3Normalize(vCurrentUp);

	_vector vTargetLook = vDir;
	vTargetLook = XMVector3Normalize(vTargetLook);

	// 새로운 right 벡터 구함
	_vector vTargetRight = XMVector3Cross(vCurrentUp, vTargetLook);

	// 새로운 right 벡터로 새로운 look 벡터를 구함
	vTargetLook = XMVector3Cross(vTargetRight, vCurrentUp);
	vTargetLook = XMVector3Normalize(vTargetLook);

	_vector vRadian = XMVector3Dot(vTargetLook, vCurrentLook);
	_float fRadian = {};

	XMStoreFloat(&fRadian, vRadian);
	fRadian = acosf(fRadian);;

	if (fRadian > 1.0f)
	{
		fRadian = 1.0f;
	}
	else if (fRadian < -1.0f)
	{
		fRadian = -1.0f;
	}

	if (fRadian <= fTurnSpeed * fTimeDelta)
	{
		vCurrentLook = vTargetLook;

		_vector vCurrentRight = XMVector3Cross(vCurrentUp, vCurrentLook);
		vCurrentLook = XMVector3Cross(vCurrentRight, vCurrentUp);
		vCurrentUp = XMVector3Cross(vCurrentLook, vCurrentRight);

		_float3 vScale = Get_Scaled();

		Set_State(STATE_RIGHT, XMVector3Normalize(vCurrentRight) * vScale.x);
		Set_State(STATE_UP, XMVector3Normalize(vCurrentUp) * vScale.y);
		Set_State(STATE_LOOK, XMVector3Normalize(vCurrentLook) * vScale.z);

		return;
	}

	_vector vCrossUp = XMVector3Cross(vCurrentLook, vTargetLook);

	_float fDot;
	XMStoreFloat(&fDot, XMVector3Dot(vCurrentUp, vCrossUp));

	if (fDot < 0)
		fTurnSpeed *= -1;

	// 쿼터니언 생성
	XMVECTOR vRotQuaternion = XMQuaternionRotationAxis(vCurrentUp, fTurnSpeed * fTimeDelta);

	// 회전 행렬 생성
	XMMATRIX RotationMatrix = XMMatrixRotationQuaternion(vRotQuaternion);

	XMMATRIX WorldMatirx = XMLoadFloat4x4(&m_WorldMatrix) * RotationMatrix;
	_float4x4 World4x4;
	XMStoreFloat4x4(&World4x4, WorldMatirx);

	// 상태 업데이트
	Set_State(STATE_RIGHT, XMLoadFloat4((_float4*)&World4x4.m[0]));
	Set_State(STATE_UP, XMLoadFloat4((_float4*)&World4x4.m[1]));
	Set_State(STATE_LOOK, XMLoadFloat4((_float4*)&World4x4.m[2]));
}

void CTransform::Turn_RollPitchYaw_Lerp(_float fPitch, _float fYaw, _float fRoll, _float fSpeed, _float fTimeDelta)
{
	_Vec4 CurrentQuat = XMQuaternionRotationMatrix(m_WorldMatrix);	//현재 회전 값

	float targetPitch = XMConvertToRadians(fPitch);	//x
	float targetYaw = XMConvertToRadians(fYaw);	//y
	float targetRoll = XMConvertToRadians(fRoll);	//z
	_Vec4 targetQuat = XMQuaternionRotationRollPitchYaw(targetPitch, targetYaw, targetRoll);

	_float fLerpSpeed = min(1.0f, fSpeed * fTimeDelta);
	_Vec4 interpolatedQuat = XMQuaternionSlerp(CurrentQuat, targetQuat, fLerpSpeed);

	_Matrix rotationMatrix = XMMatrixRotationQuaternion(interpolatedQuat);


	if (XMVectorGetX(XMQuaternionLength(interpolatedQuat - targetQuat)) <= 0.01f &&
		XMVectorGetY(XMQuaternionLength(interpolatedQuat - targetQuat)) <= 0.01f &&
		XMVectorGetZ(XMQuaternionLength(interpolatedQuat - targetQuat)) <= 0.01f
		)
	{
		// 회전 완료 처리
		m_isTurning = false;
	}

	// 월드 행렬에서 변환(위치) 추출
	_Vec4 position = Get_State(STATE_POSITION); // 행렬의 4번째 행이 위치 정보

	// 새 월드 행렬 생성
	_Matrix newWorldMatrix = rotationMatrix;
	m_WorldMatrix = newWorldMatrix;
	Set_State(STATE_POSITION, { position.x, position.y, position.z }); // 위치 유지


}

void CTransform::Rotation(const _Vec4& vAxis, _float fRadian)
{
	_Vec3		vScaled = Get_Scaled();

	_Vec3		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_Vec3		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_Vec3		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_Matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	m_vCurrentRot = { fX, fY, fZ };

	_Vec4		vRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(fX), XMConvertToRadians(fY), XMConvertToRadians(fZ));


	_float3		vScaled = Get_Scaled();

	_Vec4		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_Vec4		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_Vec4		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_Matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

_Vec3 CTransform::Get_CurrentRotation()
{
	//_Matrix WorldMatrix = m_WorldMatrix; // 이미 정의된 월드 행렬을 사용한다고 가정

	//// 각 축 벡터를 정규화하여 직교성 유지
	//WorldMatrix.Right().Normalize();
	//WorldMatrix.Up().Normalize();
	//WorldMatrix.Forward().Normalize();

	//_Vec3 vRot;
	//vRot.x = XMConvertToDegrees(atan2(WorldMatrix.m[1][2], WorldMatrix.m[2][2]));
	//vRot.y = XMConvertToDegrees(atan2(-WorldMatrix.m[0][2], sqrt(WorldMatrix.m[1][2] * WorldMatrix.m[1][2] + WorldMatrix.m[2][2] * WorldMatrix.m[2][2])));
	//vRot.z = XMConvertToDegrees(atan2(WorldMatrix.m[0][1], WorldMatrix.m[0][0]));

	return m_vCurrentRot;
}


void CTransform::BillBoard()
{
	_float3		vScale = Get_Scaled();

	_Vec3		vPosition = Get_State(STATE_POSITION);

	CGameObject* pCamera = m_pGameInstance->Find_Camera(m_pGameInstance->Get_CurLevelIndex());
	if (nullptr == pCamera)
		return;

	_Vec3		vCameraPos = pCamera->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_Vec3		vLook = vPosition - vCameraPos;

	if (0.f == vLook.Length())
		return;

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_Vec3		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	// Look 벡터와 Right 벡터를 외적하여 진짜 Up 벡터를 구함
	_Vec3		vUp = XMVector3Cross(vLook, vRight);

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::BillBoard_NoHeight()
{
	_Vec3		vScale = Get_Scaled();

	_Vec3		vPosition = XMVectorSetY(Get_State(STATE_POSITION), 0.f);

	_Vec3		vCameraPos = m_pGameInstance->Find_Camera(m_pGameInstance->Get_CurLevelIndex())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vCameraPos.y = 0.f;
	_Vec3		vLook = vPosition - vCameraPos;

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_Vec3		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	// Look 벡터와 Right 벡터를 외적하여 진짜 Up 벡터를 구함
	_Vec3		vUp = XMVector3Cross(vLook, vRight);

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Set_NewUp(_Vec3 vNewUp)
{
	// Up 벡터를 기반으로 월드 행렬 재 설정
	_Vec3 vLook = Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3Normalize(vLook);

	_Vec3 vRight = XMVector3Cross(vNewUp, vLook);

	vLook = XMVector3Cross(vRight, vNewUp);

	_float3 vScale = Get_Scaled();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vNewUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Set_NewLook(_Vec3 vNewLook)
{
	vNewLook.Normalize();

	_Vec3 vUp = Get_State(CTransform::STATE_UP);
	vUp.Normalize();

	_Vec3 vRight = XMVector3Cross(vUp, vNewLook);
	vRight.Normalize();

	vUp = XMVector3Cross(vNewLook, vRight);
	vUp.Normalize();

	_Vec3 vScale = Get_Scaled();

	Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	Set_State(CTransform::STATE_UP, vUp * vScale.y);
	Set_State(CTransform::STATE_LOOK, vNewLook * vScale.z);
}

void CTransform::Orbit(_vector vAxis, _vector vCenter, _float fLimit, _float fDistance, _float fTimeDelta)
{
	_vector      vRight = Get_State(STATE_RIGHT);
	_vector      vUp = Get_State(STATE_UP);
	_vector      vLook = Get_State(STATE_LOOK);

	_matrix      RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	_vector vMovedLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_vector vNonY_MovedLook = XMVectorSetY(vMovedLook, 0.f);

	_float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMovedLook), XMVector3Normalize(vNonY_MovedLook)));

	_vector		vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	if (fDot < fLimit)
		return;

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

	Set_State(STATE_POSITION, vCenter - XMVector3Normalize(Get_State(STATE_LOOK)) * fDistance);

}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
