#include "RigidBody.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Navigation.h"

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

HRESULT CRigidBody::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	RIGIDBODY_DESC* pDesc = static_cast<RIGIDBODY_DESC*> (pArg);

	if (nullptr == pArg || nullptr == pDesc->pOwner)
		return E_FAIL;

	m_isStatic = pDesc->isStatic;
	m_PxScene = m_pGameInstance->Get_PhysXScene();
	m_pOwner = pDesc->pOwner;
	m_pOwnerTransform = pDesc->pOwnerTransform;
	m_pOwnerNavigation = pDesc->pOwnerNavigation;

	if (FAILED(Add_PxActor(pDesc)))
		return E_FAIL;

	if (FAILED(Add_PxGeometry(pDesc)))
		return E_FAIL;

	m_PxScene->addActor(*m_PxActor);

	return S_OK;
}

void CRigidBody::Update(_float fTimeDelta)
{
   // 2. 현재 PhysX 위치 가져오기
   PxTransform PlayerPxTransform = m_PxActor->getGlobalPose();
   PxVec3 vPxPlayerPos = PlayerPxTransform.p;
   _Vec3 vPos = _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z);

   // 3. 이동 가능 여부 판단
   if (!m_pOwnerNavigation->isMove(vPos + m_vVelocity * fTimeDelta)) {
       // 이동 불가: 속도를 0으로 설정
	   static_cast<PxRigidDynamic*>(m_PxActor)->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));

       // PhysX 위치를 유지 (추가 이동 방지)
       PlayerPxTransform.p = PxVec3(vPos.x, vPos.y, vPos.z);
	   static_cast<PxRigidDynamic*>(m_PxActor)->setGlobalPose(PlayerPxTransform);
   }
   else {
       // 이동 가능: 기존 속도 유지
	   static_cast<PxRigidDynamic*>(m_PxActor)->setLinearVelocity(PxVec3(m_vVelocity.x, m_vVelocity.y, m_vVelocity.z));
   }

   PlayerPxTransform = static_cast<PxRigidDynamic*>(m_PxActor)->getGlobalPose();
   vPxPlayerPos = PlayerPxTransform.p;

   m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z));

   // 4. y축 지형 보정 (네비게이션)
   vPxPlayerPos.y = m_pOwnerNavigation->SetUp_OnCell(m_pOwnerTransform, 0.f, fTimeDelta);

   // 5. y축 보정 후 위치 동기화
   PlayerPxTransform.p = vPxPlayerPos;
   static_cast<PxRigidDynamic*>(m_PxActor)->setGlobalPose(PlayerPxTransform);

   // 6. 최종 Transform 동기화
   m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z));

}

void CRigidBody::Set_Velocity(const _Vec3& vVelocity)
{
	m_vVelocity = vVelocity;
}


HRESULT CRigidBody::Add_PxActor(RIGIDBODY_DESC* pDesc)
{
	PxPhysics* pPhysx = m_pGameInstance->Get_PhysX();

	PxTransform Transform = ConvertToPxTransform((_Vec3)m_pOwnerTransform->Get_State(CTransform::STATE_POSITION), m_pOwnerTransform->Get_Quaternion());
	if (m_isStatic)
	{
		m_PxActor = pPhysx->createRigidStatic(Transform);
	}
	else
	{
		m_PxActor = pPhysx->createRigidDynamic(Transform);
		if (!pDesc->isGravity)
		{
			m_PxActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		}
	}
	m_PxActor->setActorFlag(PxActorFlag::eVISUALIZATION, true);

	m_PxMaterial = pPhysx->createMaterial(pDesc->fStaticFriction, pDesc->fDynamicFriction, pDesc->fRestituion);

	return S_OK;
}

HRESULT CRigidBody::Add_PxGeometry(RIGIDBODY_DESC* pDesc)
{
	PxPhysics* pPhysx = m_pGameInstance->Get_PhysX();

	PxShapeFlags eShapeFlags = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

	physX::Geometry* pGeometry = pDesc->pGeometry;

	switch (pGeometry->GetType())
	{
	case physX::PX_CAPSULE:
	{
		physX::GeometryCapsule* CapsuleGeometry = static_cast<physX::GeometryCapsule*>(pGeometry);
		m_PxShape = pPhysx->createShape(PxCapsuleGeometry(CapsuleGeometry->fRadius, CapsuleGeometry->fHeight * 0.5f), *m_PxMaterial, false, eShapeFlags);
	}
		break;
	case physX::PX_SPHERE:
	{
		physX::GeometrySphere* SphereGeometry = static_cast<physX::GeometrySphere*>(pGeometry);
		m_PxShape = pPhysx->createShape(PxSphereGeometry(SphereGeometry->fRadius), *m_PxMaterial, false, eShapeFlags);
	}
		break;
	case physX::PX_BOX:
	{
		physX::GeometryBox* BoxGeometry = static_cast<physX::GeometryBox*>(pGeometry);
		m_PxShape = pPhysx->createShape(PxBoxGeometry(BoxGeometry->vSize.x * 0.5f, BoxGeometry->vSize.y * 0.5f, BoxGeometry->vSize.z * 0.5f), *m_PxMaterial, false, eShapeFlags);
	}
		break;
	}

	m_PxActor->attachShape(*m_PxShape);

	return S_OK;
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

	if (m_PxActor && m_PxShape)
	{
		// Shape detach 및 Actor 제거
		if (m_PxActor->getNbShapes() > 0)
		{
			m_PxShape->userData = nullptr;
			m_PxActor->detachShape(*m_PxShape);
		}
		m_PxScene->removeActor(*m_PxActor);

		// Actor 해제
		PhysX_RELEASE(m_PxActor);
	}
}
