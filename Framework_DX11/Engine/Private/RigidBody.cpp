#include "RigidBody.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Navigation.h"
#include "Model.h"
#include "Mesh.h"

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

	m_pPhysX = m_pGameInstance->Get_PhysX();
	m_PxScene = m_pGameInstance->Get_PhysXScene();
	m_PxCudaContextManager = m_pGameInstance->Get_PhysXCuda();

	m_isStatic = pDesc->isStatic;
	m_isOnCell = pDesc->isOnCell;
	m_isLockCell = pDesc->isLockCell;
	m_isUseClient = pDesc->isUseClient;

	m_fNavOffsetY = 0.f;

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
	if (m_isStatic)
		return;

	PxRigidDynamic* pRigidDynamic = static_cast<PxRigidDynamic*>(m_PxActor);
	if (m_pOwnerNavigation != nullptr)
	{
		PxTransform PlayerPxTransform = m_PxActor->getGlobalPose();
		PxVec3 vPxPlayerPos = PlayerPxTransform.p;
		_Vec3 vPos = _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z);

		PlayerPxTransform.p = PlayerPxTransform.p;
		m_PxActor->setGlobalPose(PlayerPxTransform);

		if (m_isLockCell && !m_pOwnerNavigation->isMove(vPos + m_vVelocity * fTimeDelta))
		{
			pRigidDynamic->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));

			PlayerPxTransform.p = PxVec3(vPos.x, vPos.y, vPos.z);
			pRigidDynamic->setGlobalPose(PlayerPxTransform);
		}
		else
		{

			//if (m_vForce.Length() > 0.f)
			//{
			   // PxVec3 vForce = ConvertToPxVec3(m_vForce);
			   // pRigidDynamic->addForce(vForce, )
			//}

			pRigidDynamic->setLinearVelocity(PxVec3(m_vVelocity.x, m_vVelocity.y, m_vVelocity.z));
		}

		PlayerPxTransform = pRigidDynamic->getGlobalPose();
		vPxPlayerPos = PlayerPxTransform.p;

		if (m_isOnCell)
		{
			vPxPlayerPos.y = m_pOwnerNavigation->SetUp_OnCell(m_pOwnerTransform, m_fNavOffsetY, fTimeDelta);
			PlayerPxTransform.p = vPxPlayerPos;
		}
		pRigidDynamic->setGlobalPose(PlayerPxTransform);

		m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z));

		PlayerPxTransform.p = PlayerPxTransform.p;
		m_PxActor->setGlobalPose(PlayerPxTransform);


	}
	else
	{
		PxTransform Transform{};
		if (m_isUseClient)
		{
			Transform = ConvertToPxTransform((_Vec3)m_pOwnerTransform->Get_State(CTransform::STATE_POSITION), m_pOwnerTransform->Get_Quaternion());
			m_PxActor->setGlobalPose(Transform);
		}

		if (m_isUseForce && m_vForce.Length() > 0.f)
		{
			PxVec3 vForce = ConvertToPxVec3(m_vForce);
			pRigidDynamic->addForce(vForce);
			m_vForce = g_XMZero;
		}
		else if(!m_isUseForce)
		{
			pRigidDynamic->setLinearVelocity(PxVec3(m_vVelocity.x, m_vVelocity.y, m_vVelocity.z));
		}

		Transform = pRigidDynamic->getGlobalPose();
		PxVec3 vPxPos = Transform.p;

		m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, _Vec3(vPxPos.x, vPxPos.y, vPxPos.z));
	}
}

void CRigidBody::Set_Velocity(const _Vec3& vVelocity)
{
	m_vVelocity = vVelocity;
}

void CRigidBody::Add_Velocity(const _Vec3& vVelocity)
{
	m_vVelocity += vVelocity;
}

void CRigidBody::Add_Force(const _Vec3& vForce, PxForceMode::Enum _eMode)
{
	m_vForce = vForce;
	m_isUseForce = true;
	//static_cast<PxRigidDynamic*>(m_PxActor)->addForce(_vForce, _eMode, _isAutowake);
}

void CRigidBody::Set_Mass(_float fMass)
{
	if (m_isStatic)
		return;

	static_cast<PxRigidDynamic*>(m_PxActor)->setMass(fMass);
}

void CRigidBody::Set_GloblePose(const _Vec3& vPos)
{
	PxTransform PxTransform = m_PxActor->getGlobalPose();
	PxVec3 vPxPos = ConvertToPxVec3(vPos);
	PxTransform.p = vPxPos;

	m_PxActor->setGlobalPose(PxTransform);
	m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, _Vec3(vPos.x, vPos.y, vPos.z));
}

void CRigidBody::Set_Gravity(_bool isGravity)
{
	m_PxActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !isGravity);
}

void CRigidBody::Set_Kinematic(_bool isKinematic)
{
	if (isKinematic)
		m_PxScene->removeActor(*m_PxActor);
	/*static_cast<PxRigidDynamic*>(m_PxActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
	m_PxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isKinematic);
	m_PxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isKinematic);*/
}

HRESULT CRigidBody::Add_PxActor(RIGIDBODY_DESC* pDesc)
{

	PxTransform Transform = ConvertToPxTransform((_Vec3)m_pOwnerTransform->Get_State(CTransform::STATE_POSITION), m_pOwnerTransform->Get_Quaternion());

	//pxcloth
	//PxCudaContextManagerDesc;
	//PxSoftBody* cloth = m_pPhysX->createSoftBody(*m_PxCudaContextManager);
	//m_PxScene->addActor(*cloth);

	//if (pDesc->isCapsule)
	//{
	//	PxQuat qq = PxQuat(PxPiDivTwo, PxVec3(0.0f, 0.0f, 1.0f)); // Z축 기준으로 90도 회전
	//	Transform.q = qq;
	//}

	if (m_isStatic)
	{
		m_PxActor = m_pPhysX->createRigidStatic(Transform);
	}
	else
	{
		m_PxActor = m_pPhysX->createRigidDynamic(Transform);
		if (!pDesc->isGravity)
		{
			m_PxActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		}


		static_cast<PxRigidDynamic*>(m_PxActor)->setRigidDynamicLockFlags(pDesc->PxLockFlags);
	}

	m_PxActor->setActorFlag(PxActorFlag::eVISUALIZATION, true);

	m_PxMaterial = m_pPhysX->createMaterial(pDesc->fStaticFriction, pDesc->fDynamicFriction, pDesc->fRestituion);

	return S_OK;
}

HRESULT CRigidBody::Add_PxGeometry(RIGIDBODY_DESC* pDesc)
{
	PxShapeFlags eShapeFlags = PxShapeFlag::eSIMULATION_SHAPE ;


	eShapeFlags = eShapeFlags | PxShapeFlag::eVISUALIZATION;


	physX::Geometry* pGeometry = pDesc->pGeometry;

	switch (pGeometry->GetType())
	{
	case physX::PX_CAPSULE:
	{
		physX::GeometryCapsule* CapsuleGeometry = static_cast<physX::GeometryCapsule*>(pGeometry);
		m_PxShape = m_pPhysX->createShape(PxCapsuleGeometry(CapsuleGeometry->fRadius, CapsuleGeometry->fHeight * 0.5f), *m_PxMaterial, false, eShapeFlags);

		PxVec3 newPosition(CapsuleGeometry->fHeight * 0.5f, 0.f, 0.f); // 캡슐의 새로운 로컬 위치
		PxQuat newRotation(PxIdentity);        // 기본 회전 (필요에 따라 수정)

		PxTransform newLocalPose(newPosition, newRotation);

		PxTransform Transform = m_PxActor->getGlobalPose();

		if (pDesc->isCapsule)
		{
			PxQuat qq = PxQuat(PxPiDivTwo, PxVec3(0.0f, 0.0f, 1.0f)); // Z축 기준으로 90도 회전
			Transform.q = qq;
			m_vOffset = { 0.f,CapsuleGeometry->fHeight , 0.f };
		}

		m_PxActor->setGlobalPose(Transform);

		m_PxShape->setLocalPose(newLocalPose);
	}
	break;
	case physX::PX_SPHERE:
	{
		//PlayerPxTransform = PxTransformFromSegment(PlayerPxTransform.p, PlayerPxTransform.p + PxVec3(0.f, 1.f, 0.f) * 1.5f);
		physX::GeometrySphere* SphereGeometry = static_cast<physX::GeometrySphere*>(pGeometry);
		m_PxShape = m_pPhysX->createShape(PxSphereGeometry(SphereGeometry->fRadius), *m_PxMaterial, false, eShapeFlags);

	}
	break;
	case physX::PX_BOX:
	{
		physX::GeometryBox* BoxGeometry = static_cast<physX::GeometryBox*>(pGeometry);
		m_PxShape = m_pPhysX->createShape(PxBoxGeometry(BoxGeometry->vSize.x * 0.5f, BoxGeometry->vSize.y * 0.5f, BoxGeometry->vSize.z * 0.5f), *m_PxMaterial, false, eShapeFlags);
	}
	break;
	case physX::PX_MODEL:
	{
		physX::GeometryTriangleMesh* TriangleGeometry = static_cast<physX::GeometryTriangleMesh*>(pGeometry);
		CModel* pModel = TriangleGeometry->pModel;

		_Matrix WorldMatrix = m_pOwnerTransform->Get_WorldMatrix();

		_Vec3 vScale;
		_Quaternion Quat;
		_Vec3 vPos;

		WorldMatrix.Decompose(vScale, Quat, vPos);	//각 요소 추출

		_Matrix ScaleMatrix = XMMatrixScalingFromVector(vScale);

		// 모델의 각 메쉬에 대해 처리
		for (auto* Mesh : pModel->Get_Meshes())
		{
			_Vec3* vVertexPositions = Mesh->Get_VertexPositions();

			_uint* pIB = Mesh->Get_Indices();  // 인덱스 데이터

			_uint iNumVertices = Mesh->Get_NumVertices();  // 정점 개수
			_uint iNumIndices = Mesh->Get_NumIndices();  // 인덱스 개수
			_uint iNumTriangles = iNumIndices / 3;  // 삼각형 개수

			// 정점 좌표를 저장할 피직스용 벡터
			vector<PxVec3> Vertices;
			Vertices.reserve(iNumVertices);

			// 정점 좌표 변환 및 저장
			for (_uint i = 0; i < iNumVertices; ++i)
			{
				Vertices.emplace_back(ConvertToPxVec3(XMVector3TransformCoord(vVertexPositions[i], WorldMatrix)));
			}

			// 인덱스를 저장할 벡터
			vector<PxU32> Indices;
			Indices.reserve(iNumIndices);

			// 인덱스 데이터 저장
			for (_uint i = 0; i < iNumIndices; ++i)
			{
				_uint Index = pIB[i];
				Indices.push_back(Index);
			}

			// 삼각형 메쉬 생성
			PxTriangleMeshDesc tDesc;
			tDesc.points.count = iNumVertices;
			tDesc.points.stride = sizeof(PxVec3);
			tDesc.points.data = Vertices.data();
			tDesc.triangles.count = iNumTriangles;
			tDesc.triangles.stride = sizeof(PxU32) * 3;
			tDesc.triangles.data = Indices.data();

			PxTolerancesScale PxScale;
			PxCookingParams PxParams(PxScale);

			PxDefaultMemoryOutputStream OutPutStream;
			if (!PxCookTriangleMesh(PxParams, tDesc, OutPutStream))
				return E_FAIL;

			PxDefaultMemoryInputData InputStream(OutPutStream.getData(), OutPutStream.getSize());

			// 삼각형 메쉬 생성
			PxTriangleMesh* pTriangleMesh = m_pPhysX->createTriangleMesh(InputStream);
			if (!pTriangleMesh)
				return E_FAIL;

			// 삼각형 메쉬 형상 생성
			PxTriangleMeshGeometry pGeometry = PxTriangleMeshGeometry(pTriangleMesh);
			m_PxShape = m_pPhysX->createShape(pGeometry, *m_PxMaterial, true, eShapeFlags);
			if (!m_PxShape)
				return E_FAIL;
		}
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
