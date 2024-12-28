#include "PhysX_Manager.h"
#include "GameInstance.h"

#include "Transform.h"

#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"

CPhysX_Manager::CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CPhysX_Manager::Initialize()
{
    // PhysX Foundation 생성
    // PhysX와 관련된 모든 객체들의 기반이 되는 싱글톤 객체
    m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

    //CUresult result = cuInit(0);
    //if (result != CUDA_SUCCESS)
    //{
    //    return -1;
    //}

    //int deviceCount = 0;
    //cudaError_t cudaErr = cudaGetDeviceCount(&deviceCount);
    //if (cudaErr != cudaSuccess || deviceCount == 0) {
    //    // CUDA 디바이스가 없는 경우
    //    return -1;
    //}

    //cudaErr = cudaSetDevice(0);
    //if (cudaErr != cudaSuccess) {
    //    return -1;
    //}
    //cudaErr = cudaFree(0); // CUDA 컨텍스트 초기화
    //if (cudaErr != cudaSuccess) {
    //    return -1;
    //}

    //CUdevice cuDevice;
    //result = cuDeviceGet(&cuDevice, 0);  // 0번 디바이스 선택
    //if (result != CUDA_SUCCESS) {
    //    return -1;
    //}
    //cudaErr = cudaSetDevice(cuDevice);

    //cudaGetDevice(&deviceCount);

    //CUcontext m_pCudaContext = nullptr;
    //result = cuCtxCreate(&m_pCudaContext, 0, cuDevice);  // 올바른 장치를 설정하여 컨텍스트 생성
    //if (result != CUDA_SUCCESS) {
    //    return -1;
    //}

    //// Cuda를 사용한 GPU 물리 연산
    //PxCudaContextManagerDesc cudaContextManagerDesc; 
    //cudaContextManagerDesc.graphicsDevice = m_pDevice;    
    //cudaContextManagerDesc.ctx = &m_pCudaContext;

    //cudaErr = cudaGetLastError();

    //m_CudaContextManager = PxCreateCudaContextManager(*m_PxFoundation, cudaContextManagerDesc);
    //if (m_CudaContextManager)
    //{
    //    if (!m_CudaContextManager->contextIsValid())
    //    {
    //        PX_RELEASE(m_CudaContextManager);
    //    }
    //       
    //}
    // PVD(PhysX Visual Debugger) 생성
    m_Pvd = PxCreatePvd(*m_PxFoundation);
    // PVD 소켓 연결 설정
    m_pTransport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_iPvdPortNumber, m_iPvdTimeOutSeconds);
    m_Pvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

    // PhysX SDK 생성
    // 피직스 객체들을 생성할 수 있는 싱글톤 팩토리 클래스
    // 씬이나 매터리얼을 생성하거나 물리 객체의 형체 정보(Shape), 강체 인스턴스 등 거의 대부분의 PhysX 객체들을 생성할 때 사용
    m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);
    PxInitExtensions(*m_PhysX, m_Pvd);

    // 씬 설명 설정
    PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

    // CPU 디스패처(워커 스레드) 생성
    m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    sceneDesc.solverBatchSize = 256;
    sceneDesc.solverArticulationBatchSize = 32;

    sceneDesc.contactReportStreamBufferSize = 16384;
    sceneDesc.contactPairSlabSize = 512;

    // GPU 관련 설정
    sceneDesc.cudaContextManager = m_CudaContextManager;
    sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
    sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
    sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
    sceneDesc.gpuMaxNumPartitions = 8;
    sceneDesc.gpuMaxNumStaticPartitions = 255;

    // 매 프레임마다 마찰 계산 - 필요할까?
    //sceneDesc.flags |= PxSceneFlag::eENABLE_FRICTION_EVERY_ITERATION;

    // 씬 생성 PhysX 액터들이 배치될 수 있는 씬
    m_PxScene = m_PhysX->createScene(sceneDesc);

    // PVD 클라이언트 설정
    // Physx Visual Debugger 프로그램에 한 씬의 정보를 전달할 때 어떤 정보를 전달해 줄지 플래그를 설정해주는 객체
    m_PvdClient = m_PxScene->getScenePvdClient();
    if (m_PvdClient)
    {
        // PVD 클라이언트 플래그 설정
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //강체 관련
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // 충돌 관련
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene 쿼리
    }

    // Material은 물리적인 표면 재질의 상태를 나타내는 객체
    // 표면의 정지마찰계수, 운동마찰계수, 반발계수 등 표면 재질의 다양한 성질을 지정할 수 있다
    //gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    //PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    //gScene->addActor(*groundPlane);


    return S_OK;
}

void CPhysX_Manager::PhysX_Update(_float fTimeDelta)
{
   

    //for (_uint i = 0; i < m_Monsters.size(); ++i)
    //{
    //    CRigidBody* pMonsterRigidBody = dynamic_cast<CRigidBody*>(m_Monsters[i]->Find_Component(RIGIDBODY));
    //    CNavigation* pNavigation = dynamic_cast<CNavigation*>(m_Monsters[i]->Find_Component(NAVIGATION));

    //    // 1. 현재 속도 가져오기
    //    _Vec3 vVelocity = pMonsterRigidBody->Get_Velocity();

    //    // 2. 현재 PhysX 위치 가져오기
    //    PxTransform MonsterPxTransform = m_MonsterRigids[i]->getGlobalPose();
    //    PxVec3 vPxMonsterPos = MonsterPxTransform.p;
    //    _Vec3 vPos = _Vec3(vPxMonsterPos.x, vPxMonsterPos.y, vPxMonsterPos.z);
    //   
    //    // 3. 이동 가능 여부 판단
    //    if (!pNavigation->isMove(vPos + vVelocity * fTimeDelta)) {
    //        // 이동 불가: 속도를 0으로 설정
    //        m_MonsterRigids[i]->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));

    //        // PhysX 위치를 유지 (추가 이동 방지)
    //        MonsterPxTransform.p = PxVec3(vPos.x, vPos.y, vPos.z);
    //        m_MonsterRigids[i]->setGlobalPose(MonsterPxTransform);
    //    }
    //    else {
    //        // 이동 가능: 기존 속도 유지
    //        m_MonsterRigids[i]->setLinearVelocity(PxVec3(vVelocity.x, vVelocity.y, vVelocity.z));
    //    }

    //    MonsterPxTransform = m_MonsterRigids[i]->getGlobalPose();
    //    vPxMonsterPos = MonsterPxTransform.p;

    //    m_Monsters[i]->Get_Transform()->Set_State(CTransform::STATE_POSITION, _Vec3(vPxMonsterPos.x, vPxMonsterPos.y, vPxMonsterPos.z));

    //    // 4. y축 지형 보정 (네비게이션)
    //    vPxMonsterPos.y = pNavigation->SetUp_OnCell(m_pPlayer->Get_Transform(), 0.f, fTimeDelta);

    //    // 5. y축 보정 후 위치 동기화
    //    MonsterPxTransform.p = vPxMonsterPos;
    //    m_MonsterRigids[i]->setGlobalPose(MonsterPxTransform);

    //    // 6. 최종 Transform 동기화
    //    m_Monsters[i]->Get_Transform()->Set_State(CTransform::STATE_POSITION, _Vec3(vPxMonsterPos.x, vPxMonsterPos.y, vPxMonsterPos.z));
    //}

    //CRigidBody* pPlayerRigidBody = dynamic_cast<CRigidBody*>(m_pPlayer->Find_Component(RIGIDBODY));
    //CNavigation* pNavigation = dynamic_cast<CNavigation*>(m_pPlayer->Find_Component(NAVIGATION));

    //// 1. 현재 속도 가져오기
    //_Vec3 vVelocity = pPlayerRigidBody->Get_Velocity();

    //// 2. 현재 PhysX 위치 가져오기
    //PxTransform PlayerPxTransform = m_pPlayerRigid->getGlobalPose();
    //PxVec3 vPxPlayerPos = PlayerPxTransform.p;
    //_Vec3 vPos = _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z);

    //// 3. 이동 가능 여부 판단
    //if (!pNavigation->isMove(vPos + vVelocity * fTimeDelta)) {
    //    // 이동 불가: 속도를 0으로 설정
    //    m_pPlayerRigid->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));

    //    // PhysX 위치를 유지 (추가 이동 방지)
    //    PlayerPxTransform.p = PxVec3(vPos.x, vPos.y, vPos.z);
    //    m_pPlayerRigid->setGlobalPose(PlayerPxTransform);
    //}
    //else {
    //    // 이동 가능: 기존 속도 유지
    //    m_pPlayerRigid->setLinearVelocity(PxVec3(vVelocity.x, vVelocity.y, vVelocity.z));
    //}

    //PlayerPxTransform = m_pPlayerRigid->getGlobalPose();
    //vPxPlayerPos = PlayerPxTransform.p;

    //m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z));

    //// 4. y축 지형 보정 (네비게이션)
    //vPxPlayerPos.y = pNavigation->SetUp_OnCell(m_pPlayer->Get_Transform(), 0.f, fTimeDelta);

    //// 5. y축 보정 후 위치 동기화
    //PlayerPxTransform.p = vPxPlayerPos;
    //m_pPlayerRigid->setGlobalPose(PlayerPxTransform);

    //// 6. 최종 Transform 동기화
    //m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, _Vec3(vPxPlayerPos.x, vPxPlayerPos.y, vPxPlayerPos.z));


    // 물리 씬 업데이트
    m_PxScene->simulate(fTimeDelta);
    m_PxScene->fetchResults(true);
}

_bool CPhysX_Manager::RayCast(PxVec3 vRayPos, PxVec3 vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _bool isHit = false;

    PxRaycastHit hit;
    PxRaycastBuffer hitBuffer(&hit);
    PxReal fLength = 100.f;

    PxVec3 vRayEnd = vRayPos + vRayDir * fLength;

    PxQueryFilterData filterData(PxQueryFlag::eSTATIC);

    // 레이캐스트 수행
    // 가장 처음 충돌한 것을 반환 eMESH_ANY
    // 스태틱 개체에만 반응 eSTATIC
    if (isHit = m_PxScene->raycast(vRayPos, vRayDir, fLength, hitBuffer, PxHitFlag::eMESH_ANY, filterData))
    {
        hit = hitBuffer.block;
        PxShape* shape = hitBuffer.block.shape;

        if (shape)
        {
            const PxGeometryHolder geometry = shape->getGeometry();
            PxGeometryType::Enum eType = geometry.getType();

            if (eType == PxGeometryType::eTRIANGLEMESH)
            {
                const PxTriangleMeshGeometry& triangleMeshGeom = geometry.triangleMesh();
                PxTriangleMesh* triangleMesh = triangleMeshGeom.triangleMesh;

                // 충돌 지점의 위치
                PxVec3 collisionPoint = hitBuffer.block.position;

                // 삼각형의 정점 정보를 가져오기
                PxU32 triangleIndex = hitBuffer.block.faceIndex;
                const void* indexBuffer = triangleMesh->getTriangles();
                const PxVec3* vertices = triangleMesh->getVertices();

                // 삼각형 인덱스 타입에 따른 캐스팅
                PxU32 idx0, idx1, idx2;

                if (triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES) {
                    const PxU16* indices = reinterpret_cast<const PxU16*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }
                else {
                    const PxU32* indices = reinterpret_cast<const PxU32*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }

                // 삼각형의 정점 위치 가져오기
                PxVec3 v0 = vertices[idx0];
                PxVec3 v1 = vertices[idx1];
                PxVec3 v2 = vertices[idx2];

                // 삼각형의 법선 계산

                // 두 벡터를 생성
                PxVec3 edge1 = v1 - v0;
                PxVec3 edge2 = v2 - v1;

                PxVec3 normal = edge1.cross(edge2);
                normal.normalize(); // 법선을 정규화

                *vNormal = XMVectorSet(normal.x, normal.y, normal.z, 0.f);
                *vHitPos = XMVectorSet(hitBuffer.block.position.x, hitBuffer.block.position.y, hitBuffer.block.position.z, 1.f);
                *fHitDistance = hitBuffer.block.distance;
            }
        }
    }

#ifdef _DEBUG

    if (m_PvdClient && isHit)
    {
        PxDebugLine lines = { PxDebugLine(vRayPos, hitBuffer.block.position, PxDebugColor::eARGB_YELLOW) };
        m_PvdClient->drawLines(&lines, 1);
    }
#endif

    return isHit;
}

_bool CPhysX_Manager::RayCast(_vector vRayPos, _vector vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _bool isHit = false;

    PxRaycastHit hit;
    PxRaycastBuffer hitBuffer(&hit);
    PxReal fLength = 100.f;

    //PxVec3 vRayEnd = vRayPos + vRayDir * fLength;

    PxVec3 PXvRayPos = PxVec3(XMVectorGetX(vRayPos), XMVectorGetY(vRayPos), XMVectorGetZ(vRayPos));
    PxVec3 PXvRayDir = PxVec3(XMVectorGetX(vRayDir), XMVectorGetY(vRayDir), XMVectorGetZ(vRayDir));

    PxQueryFilterData filterData(PxQueryFlag::eSTATIC);

    // 레이캐스트 수행
    // 가장 처음 충돌한 것을 반환 eMESH_ANY
    // 스태틱 개체에만 반응 eSTATIC
    if (isHit = m_PxScene->raycast(PXvRayPos, PXvRayDir, fLength, hitBuffer, PxHitFlag::eMESH_ANY, filterData))
    {
        hit = hitBuffer.block;
        PxShape* shape = hitBuffer.block.shape;

        if (shape)
        {
            const PxGeometryHolder geometry = shape->getGeometry();
            PxGeometryType::Enum eType = geometry.getType();

            if (eType == PxGeometryType::eTRIANGLEMESH)
            {
                const PxTriangleMeshGeometry& triangleMeshGeom = geometry.triangleMesh();
                PxTriangleMesh* triangleMesh = triangleMeshGeom.triangleMesh;

                // 충돌 지점의 위치
                PxVec3 collisionPoint = hitBuffer.block.position;

                // 삼각형의 정점 정보를 가져오기
                PxU32 triangleIndex = hitBuffer.block.faceIndex;
                const void* indexBuffer = triangleMesh->getTriangles();
                const PxVec3* vertices = triangleMesh->getVertices();

                // 삼각형 인덱스 타입에 따른 캐스팅
                PxU32 idx0, idx1, idx2;

                if (triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES) {
                    const PxU16* indices = reinterpret_cast<const PxU16*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }
                else {
                    const PxU32* indices = reinterpret_cast<const PxU32*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }

                // 삼각형의 정점 위치 가져오기
                PxVec3 v0 = vertices[idx0];
                PxVec3 v1 = vertices[idx1];
                PxVec3 v2 = vertices[idx2];

                // 삼각형의 법선 계산

                // 두 벡터를 생성
                PxVec3 edge1 = v1 - v0;
                PxVec3 edge2 = v2 - v1;

                PxVec3 normal = edge1.cross(edge2);
                normal.normalize(); // 법선을 정규화

                *vNormal = XMVectorSet(normal.x, normal.y, normal.z, 0.f);
                *vHitPos = XMVectorSet(hitBuffer.block.position.x, hitBuffer.block.position.y, hitBuffer.block.position.z, 1.f);
                *fHitDistance = hitBuffer.block.distance;
            }
        }
    }

#ifdef _DEBUG

    if (m_PvdClient && isHit)
    {
        PxDebugLine lines = { PxDebugLine(PXvRayPos, hitBuffer.block.position, PxDebugColor::eARGB_YELLOW) };
        m_PvdClient->drawLines(&lines, 1);
    }
#endif

    return isHit;
}

PxTransform CPhysX_Manager::Get_PhysXTransform(CGameObject* pObject)
{
    //이게 맞나?
    //암튼 피직스용 Transfrom으로 변환
    const _float4x4* WorldMat = pObject->Get_Transform()->Get_WorldMatrix_Ptr();

    PxVec3 position(WorldMat->_41, WorldMat->_42, WorldMat->_43);

    // 회전 추출 (3x3 상단 행렬을 쿼터니언으로 변환)
    PxMat33 rotation(
        PxVec3(WorldMat->_11, WorldMat->_12, WorldMat->_13),
        PxVec3(WorldMat->_21, WorldMat->_22, WorldMat->_23),
        PxVec3(WorldMat->_31, WorldMat->_32, WorldMat->_33)
    );
    PxQuat quaternion = PxQuat(rotation);

    return PxTransform(position, quaternion);
}

CPhysX_Manager* CPhysX_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPhysX_Manager* pInstance = new CPhysX_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CPhysX_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysX_Manager::Free()
{
    __super::Free();

    PhysX_RELEASE(m_PxScene);

    // PhysX Extensions 닫기
    PxCloseExtensions();

    PhysX_RELEASE(m_PxDispatcher);
    PhysX_RELEASE(m_PhysX);


    PxPvdTransport* transport = m_Pvd->getTransport();
    m_pTransport->disconnect();
    PhysX_RELEASE(m_Pvd);
    PhysX_RELEASE(m_pTransport);

    PhysX_RELEASE(m_CudaContextManager);
    // 제일 마지막에 Release
    PhysX_RELEASE(m_PxFoundation);

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}