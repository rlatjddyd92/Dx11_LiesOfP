#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice,  pContext }
{
	m_eComponentType = RIGIDBODY;
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
	: CComponent{ Prototype }
{
	m_eComponentType = RIGIDBODY;
}

void CRigidBody::Set_GravityDir(_vector vDir)
{
	XMStoreFloat3(&m_vGraivtyDirection, vDir);
}

HRESULT CRigidBody::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	m_isActive = true;
	m_isGravity = true;
	m_isGround = false;
	m_isFriction = true;
	m_isKinematic = false;
	m_fGravityLimit = 5.f;

	return S_OK;
}

void CRigidBody::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	_vector vForce = XMLoadFloat3(&m_vForce);
	_vector vVelocity = XMLoadFloat3(&m_vVelocity);
	_vector vAccel = XMLoadFloat3(&m_vAccel);

	//가해지는 힘이 있을 때만 실행
	//힘을 가해 속도를 더함
	if (XMVector3Length(vForce).m128_f32[0] != 0.f)
	{
		//가속도를 이용해서 속도를 증가시킨다.
		vVelocity += vAccel;
		XMStoreFloat3(&m_vVelocity, vVelocity);
	}

	Calculate_Tranform(fTimeDelta);

	//힘 초기화
	m_vForce = { 0.f, 0.f, 0.f };
}

void CRigidBody::Calculate_Tranform(_float fTimeDelta)
{
	Calculate_Friction(fTimeDelta);

	//중력 적용 && !m_isGround
	if (m_isGravity)
	{
		Calculate_Gravity(fTimeDelta);
	}

	_vector vCurrentPosition = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	//속도 제한
	if (fabsf(m_vMaxVelocity.x) < fabsf(m_vVelocity.x))
	{
		m_vVelocity.x = (m_vVelocity.x / fabsf(m_vVelocity.x)) * fabsf(m_vMaxVelocity.x);
	}

	// 떨어지는 y만 제한
	if (m_vVelocity.y <= -m_vMaxVelocity.y)
	{
		m_vVelocity.y = (m_vVelocity.y / fabsf(m_vVelocity.y)) * fabsf(m_vMaxVelocity.y);
	}

	_vector vNewPosition = vCurrentPosition + (XMLoadFloat3(&m_vVelocity) * fTimeDelta);

	m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, vNewPosition);
}

void CRigidBody::Calculate_Gravity(_float fTimeDelta)
{
	_vector vGravity = XMLoadFloat3(&m_vGraivtyDirection) * m_fGravityScale * fTimeDelta;

	if (fabsf(XMVectorGetY(vGravity)) > m_fGravityLimit)
	{
		vGravity = XMVectorSetY(vGravity, m_fGravityLimit);
	}

	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) - vGravity);
}

void CRigidBody::Calculate_Friction(_float fTimeDelta)
{
	if (!m_isFriction)
		return;

	XMVECTOR vVelocity = XMLoadFloat3(&m_vVelocity);

	if (XMVector3Equal(vVelocity, XMVectorZero()))
	{
		return; // 속도가 0이면 마찰을 적용할 필요가 없음
	}

	// 마찰이 가해질 방향
	_vector vFricitionDir = XMVector3Normalize(vVelocity) * -1.f;
	_vector vFriction = vFricitionDir * XMLoadFloat3(&m_vFriction) * fTimeDelta;

	_float fVelocityLength = XMVectorGetX(XMVector3Length(vVelocity));
	_float fFrictionLength = XMVectorGetX(XMVector3Length(vFriction));

	//마찰이 더 큰 경우
	if (fFrictionLength >= fVelocityLength)
	{
		m_vVelocity = { 0.f, 0.f, 0.f };
	}
	else
	{
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vFriction);
	}
}

void CRigidBody::Add_Force(FORCEMODE eForceMode, _vector vForce)
{
	XMStoreFloat3(&m_vForce, vForce);

	_vector		vAccel = XMLoadFloat3(&m_vAccel);
	_vector		vVelocity = XMLoadFloat3(&m_vVelocity);

	switch (eForceMode)
	{
		//가속을 추가해줄 것임(무게를 적용)
	case Engine::CRigidBody::FORCE:
		//가속도는 = 힘 / 질량
		vAccel += vForce / m_fMass;
		XMStoreFloat3(&m_vAccel, vAccel);
		break;

		//가속을 추가해줄 것임(무게를 적용 안 함)
	case Engine::CRigidBody::ACCELERATION:
		vAccel += vForce;
		XMStoreFloat3(&m_vAccel, vAccel);
		break;

		//순간적으로 밀어줄 거임(무게를 적용)
	case Engine::CRigidBody::IMPLUSE:
		vVelocity += vForce / m_fMass;
		XMStoreFloat3(&m_vVelocity, vVelocity);
		break;

		//순간적으로 밀어줄 거임(무게를 적용 안 함)
	case Engine::CRigidBody::VELOCITYCHANGE:
		vVelocity += vForce;
		XMStoreFloat3(&m_vVelocity, vVelocity);
		break;

	case Engine::CRigidBody::MODE_END:
		break;
	default:
		break;
	}
}

void CRigidBody::Add_Force_Direction(_fvector vDirection, _fvector vMagnitude, FORCEMODE eForceMode)
{
	_vector vDir = XMVector3Normalize(vDirection);

	_float vForceX = XMVectorGetX(vDir) * XMVectorGetX(vMagnitude);    // 방향과 크기를 곱하여 힘 벡터 생성
	_float vForceY = XMVectorGetY(vDir) * XMVectorGetY(vMagnitude);
	_float vForceZ = XMVectorGetZ(vDir) * XMVectorGetZ(vMagnitude);

	_vector vForce = XMVectorSet(vForceX, vForceY, vForceZ, 0.f);

	Add_Force(eForceMode, vForce); // 기존 Add_Force 함수 이용
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();
}
