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

void CTransform::LookAt_Lerp(_Vec4 vAt, _float fSpeed, _float fTimeDelta)
{
	/* 팀원들에게 보여주시 위한 예시 */
	_Vec3 vPlayerLook = Get_State(STATE_LOOK);
	vPlayerLook.Normalize();

	_Vec3 vTargetLook = vAt;
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

void CTransform::LookAt_Lerp_NoHeight(_Vec4 vAt, _float fSpeed, _float fTimeDelta)
{	
	/* 팀원들에게 보여주시 위한 예시2 */
	_Vec3 vPlayerLook = Get_State(STATE_LOOK);
	vPlayerLook.y = 0.f;
	vPlayerLook.Normalize();

	_Vec3 vTargetLook = vAt;
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
	}
	else
	{
		_Vec3 vPlayerUp = Get_State(STATE_UP);
		_Vec3 vCrossUp = vPlayerLook.Cross(vTargetLook);

		if (vPlayerUp.Dot(vCrossUp) >= 0)
		{
			Turn((_Vec4)vWorldUp, fTimeDelta, fSpeed);
		}
		else
		{
			Turn((_Vec4)vWorldUp, fTimeDelta, fSpeed);
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

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
	_Vec3		vPosition = Get_State(STATE_POSITION);
	_Vec3		vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if(nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_Vec3		vPosition = Get_State(STATE_POSITION);
	_Vec3		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_Vec3		vPosition = Get_State(STATE_POSITION);
	_Vec3		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_Vec3		vPosition = Get_State(STATE_POSITION);
	_Vec3		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

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
	m_fCurrentRotationX = fX;
	m_fCurrentRotationY = fY;
	m_fCurrentRotationZ = fZ;

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

_Vec3 CTransform::Get_Rotation_Dgree()
{
	_Vec3 vScale = Get_Scaled();

	_Matrix RotationMatrix = m_WorldMatrix;
	RotationMatrix._11 /= vScale.x;
	RotationMatrix._12 /= vScale.x;
	RotationMatrix._13 /= vScale.x;
	RotationMatrix._21 /= vScale.y;
	RotationMatrix._22 /= vScale.y;
	RotationMatrix._23 /= vScale.y;
	RotationMatrix._31 /= vScale.z;
	RotationMatrix._32 /= vScale.z;
	RotationMatrix._33 /= vScale.z;

	_Quaternion rotationQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(RotationMatrix);

	// Roll, Pitch, Yaw 계산
	float fX = atan2(2.0f * (rotationQuat.y * rotationQuat.z + rotationQuat.w * rotationQuat.x),
		rotationQuat.w * rotationQuat.w + rotationQuat.x * rotationQuat.x -
		rotationQuat.y * rotationQuat.y - rotationQuat.z * rotationQuat.z);

	float fY = asin(-2.0f * (rotationQuat.x * rotationQuat.z - rotationQuat.w * rotationQuat.y));

	float fZ = atan2(2.0f * (rotationQuat.x * rotationQuat.y + rotationQuat.w * rotationQuat.z),
		rotationQuat.w * rotationQuat.w - rotationQuat.x * rotationQuat.x -
		rotationQuat.y * rotationQuat.y + rotationQuat.z * rotationQuat.z);

	// 도 단위로 변환하여 반환
	return DirectX::SimpleMath::Vector3(
		DirectX::XMConvertToDegrees(fX),
		DirectX::XMConvertToDegrees(fY),
		DirectX::XMConvertToDegrees(fZ)
	);
}

void CTransform::BillBoard()
{
	_float3		vScale = Get_Scaled();

	_Vec3		vPosition = Get_State(STATE_POSITION);

	_Vec3		vCameraPos = m_pGameInstance->Find_Camera(m_pGameInstance->Get_CurLevelIndex())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
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
