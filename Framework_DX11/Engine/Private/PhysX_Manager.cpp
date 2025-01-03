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
    m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_ErrorCallback);

    // PVD(PhysX Visual Debugger) 생성
    m_Pvd = PxCreatePvd(*m_PxFoundation);
    // PVD 소켓 연결 설정
    m_pTransport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_iPvdPortNumber, m_iPvdTimeOutSeconds);
    m_Pvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

    // 피직스 객체들을 생성할 수 있는 싱글톤 팩토리 클래스
    // 씬이나 매터리얼을 생성하거나 물리 객체의 형체 정보(Shape), 강체 인스턴스 등 거의 대부분의 PhysX 객체들을 생성할 때 사용
    m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);

    // 씬 설명 설정
    PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

    // CPU 디스패처(워커 스레드) 생성
    m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.staticStructure = PxPruningStructureType::eDYNAMIC_AABB_TREE;

    sceneDesc.solverBatchSize = 512;
    sceneDesc.solverArticulationBatchSize = 128;

    sceneDesc.contactReportStreamBufferSize = 16384;
    sceneDesc.contactPairSlabSize = 512;
    
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD; 
    sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
    sceneDesc.flags |= PxSceneFlag::eENABLE_AVERAGE_POINT;
    sceneDesc.flags |= PxSceneFlag::eENABLE_FRICTION_EVERY_ITERATION;

#ifdef __cuda_cuda_h__
    // GPU 관련 설정
    if (S_OK == Create_CudaContextManager())
    {
        sceneDesc.cudaContextManager = m_CudaContextManager;
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
        sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
        sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
        sceneDesc.solverType = PxSolverType::eTGS;
        sceneDesc.gpuMaxNumPartitions = 8;
        sceneDesc.gpuMaxNumStaticPartitions = 255;
    }
#endif // __cuda_cuda_h__

    // 씬 생성 PhysX 액터들이 배치될 수 있는 씬
    m_PxScene = m_PhysX->createScene(sceneDesc);

    // PVD 클라이언트 설정
    m_PvdClient = m_PxScene->getScenePvdClient();
    if (m_PvdClient)
    {
        // PVD 클라이언트 플래그 설정
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //강체 관련
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // 충돌 관련
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene 쿼리
    }


#ifdef _DEBUG
    m_PxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
    m_PxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
    m_PxScene->getVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES);
#endif

    return S_OK;
}

void CPhysX_Manager::PhysX_Update(_float fTimeDelta)
{


    // 물리 씬 업데이트
    m_PxScene->simulate(fTimeDelta);
    m_PxScene->fetchResults(true);
    m_PxScene->fetchResultsParticleSystem();
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

HRESULT CPhysX_Manager::Create_CudaContextManager()
{
#ifdef __cuda_cuda_h__

    _int cudaDriverVersion = 0;
    cuDriverGetVersion(&cudaDriverVersion);

    if (cudaDriverVersion < 11000)
    {
        MSG_BOX(TEXT("CUDA가 없거나 버전이 너무 낮아요."));
        return E_FAIL;
    }
    else
    {
        cudaError_t cudaErr;
        CUresult result;
        _int deviceCount = 0;

        result = cuInit(0);
        if (result != CUDA_SUCCESS)
        {
            MSG_BOX(TEXT("CUDA가 초기화 실패."));
            return E_FAIL;
        }

        cudaGetDeviceCount(&deviceCount);
        if (deviceCount == 0)
        {
            MSG_BOX(TEXT("CUDA Device가 없는데"));
            return E_FAIL;
        }

        ////Compute Capability가 3.0 미만인 GPU는 PhysX GPU Dynamics를 지원하지 않음
        //for (_int i = 0; i < deviceCount; i++) 
        //{
        //    cudaDeviceProp prop;
        //    cudaGetDeviceProperties(&prop, i);
        //    printf("Device %d: %s\n", i, prop.name);
        //    printf("  Compute Capability: %d.%d\n", prop.major, prop.minor);
        //    printf("  Total Global Memory: %lu MB\n", prop.totalGlobalMem / (1024 * 1024));
        //    printf("  Multiprocessors: %d\n", prop.multiProcessorCount);
        //}

        _int driverVersion = 0;
        cuDriverGetVersion(&driverVersion);
        printf("CUDA Driver Version: %d\n", driverVersion);

        // CUDA 디바이스 핸들 얻기
        CUdevice cuDevice;
        result = cuDeviceGet(&cuDevice, 0);  // 첫 번째 CUDA 장치 가져오기
        if (result != CUDA_SUCCESS) {
            printf("Failed to get CUDA device.\n");
            return E_FAIL;
        }

        CUcontext cuContext;
        result = cuCtxCreate(&cuContext, 0, cuDevice);  // CUDA 디바이스 핸들로 컨텍스트 생성
        if (result != CUDA_SUCCESS) {
            printf("Failed to create CUDA context.\n");
            return E_FAIL;
        }

        // CUDA 컨텍스트 활성화
        result = cuCtxPushCurrent(cuContext);
        if (result != CUDA_SUCCESS) {
            printf("Failed to push CUDA context.\n");
            return E_FAIL;
        }

        PxCudaContextManagerDesc cudaContextManagerDesc;

        m_CudaContextManager = PxCreateCudaContextManager(*m_PxFoundation, cudaContextManagerDesc, &m_ProfilerCallbakc);
        if (!m_CudaContextManager || !m_CudaContextManager->contextIsValid())
        {
            printf("Failed to create CUDA Context Manager.\n");
            return E_FAIL;
        }
        else
        {
            if (!m_CudaContextManager->contextIsValid())
            {
                PX_RELEASE(m_CudaContextManager);

                return E_FAIL;
            }
        }
    }
#endif

    return S_OK;
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

    PhysX_RELEASE(m_PxDispatcher);
    PhysX_RELEASE(m_PhysX);

    m_pTransport->disconnect();
    PhysX_RELEASE(m_Pvd);
    PhysX_RELEASE(m_pTransport);

    PhysX_RELEASE(m_CudaContextManager);

    //// PhysX Extensions 닫기
    //PxCloseExtensions();

    //// 제일 마지막에 Release
    PhysX_RELEASE(m_PxFoundation);

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}