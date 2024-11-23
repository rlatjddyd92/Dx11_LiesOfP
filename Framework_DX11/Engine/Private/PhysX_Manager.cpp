#include "PhysX_Manager.h"
#include "GameInstance.h"

#include "Transform.h"

#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"

CPhysX_Manager::CPhysX_Manager()
{
}

HRESULT CPhysX_Manager::Initialize()
{
    // PhysX Foundation 생성
    // PhysX와 관련된 모든 객체들의 기반이 되는 싱글톤 객체
    m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

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
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);

    // CPU 디스패처(워커 스레드) 생성
    m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

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
    if (nullptr == m_pPlayer)
        return;

    Compute_MonsterCollision();

    // 물리 씬 업데이트
    m_PxScene->simulate(fTimeDelta);
    m_PxScene->fetchResults(true);
}

HRESULT CPhysX_Manager::AddPhysX_StaticMesh(CGameObject* pObject, _wstring strModelName)
{
    // 게임 객체의 모델 컴포넌트를 얻음
    CModel* pModel = dynamic_cast<CModel*>(pObject->Find_Component(MODEL));

    // 모델의 각 메쉬에 대해 처리
    for (auto* Mesh : pModel->Get_Meshes())
    {
        VTXMESH* pVB = Mesh->Get_Vertices();  // 정점 데이터
        VTXANIMMESH* pAnimVB = Mesh->Get_AnimVertices();  // 정점 데이터
        _uint* pIB = Mesh->Get_Indices();  // 인덱스 데이터

        _uint iNumVertices = Mesh->Get_NumVertices();  // 정점 개수
        _uint iNumIndices = Mesh->Get_NumIndices();  // 인덱스 개수
        _uint iNumTriangles = iNumIndices / 3;  // 삼각형 개수

        // 정점 좌표를 저장할 피직스용 벡터
        vector<PxVec3> Vertices;
        Vertices.reserve(iNumVertices);

        // 게임 객체의 월드 변환 행렬
        _matrix WorldMatrix = pObject->Get_Transform()->Get_WorldMatrix();

        // 정점 좌표 변환 및 저장
        for (_uint i = 0; i < iNumVertices; ++i)
        {
            _vector vVertexPosition = {};
            if (nullptr == pVB)
                vVertexPosition = XMLoadFloat3(&pAnimVB[i].vPosition);
            else
                vVertexPosition = XMLoadFloat3(&pVB[i].vPosition);

            vVertexPosition = XMVector3TransformCoord(vVertexPosition, WorldMatrix);
            Vertices.push_back(PxVec3(XMVectorGetX(vVertexPosition), XMVectorGetY(vVertexPosition), XMVectorGetZ(vVertexPosition)));

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

        // 삼각형 메쉬 설명 생성
        PxTriangleMeshDesc tDesc;
        tDesc.points.count = iNumVertices;
        tDesc.points.stride = sizeof(PxVec3);
        tDesc.points.data = Vertices.data();
        tDesc.triangles.count = iNumTriangles;
        tDesc.triangles.stride = sizeof(PxU32) * 3;
        tDesc.triangles.data = Indices.data();

        // 삼각형 메쉬 생성
        PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(0.0f, 0.0f)), tDesc);

        // 삼각형 메쉬 형상 생성
        PxTriangleMeshGeometry* pGeometry = new PxTriangleMeshGeometry(pTriangleMesh);

        // 충돌 메쉬 목록에 추가
        m_ColMesheGeometries.push_back(pGeometry);

        // RigidStatic 객체 생성
        // 움직이지 않는 정적 모델들은 이거로 만들어야함
        PxRigidStatic* pActor = m_PhysX->createRigidStatic(Get_PhysXTransform(pObject));

        // 물리 재질 생성
        PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);
        // 충돌 형상 생성 및 추가
        PxShape* pShape = m_PhysX->createShape(*pGeometry, *Material);
        pActor->attachShape(*pShape);

        // 물리 씬에 Actor 추가
        m_PxScene->addActor(*pActor);

        // 정적 Actor 목록에 추가
        m_StaticActors.push_back(pActor);
    }

    return S_OK;
}

HRESULT CPhysX_Manager::SetUp_Player(CGameObject* pPlayer)
{
    PxRigidDynamic* pRigid = m_PhysX->createRigidDynamic(Get_PhysXTransform(pPlayer));

    // 머테리얼 환경 정의
    // 정적 마찰, 동적 마찰, 반발 계수
    PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);

    // 반지름이 0.5이고 높이가 0.3인 캡슐 헝태

    m_PlayerCapusle = PxCapsuleGeometry(0.55f, 0.29f);
    PxShape* pShape = m_PhysX->createShape(m_PlayerCapusle, *Material);

    pRigid->attachShape(*pShape);

    m_PxScene->addActor(*pRigid);
    m_pPlayerRigid = pRigid;

    m_pPlayer = pPlayer;
    Safe_AddRef(m_pPlayer);

    return S_OK;
}

HRESULT CPhysX_Manager::Add_Monster(CGameObject* pMonster, _float fRadius, _float fHalfHeight)
{
    if (nullptr == pMonster)
        return E_FAIL;

    PxRigidDynamic* pRigid = m_PhysX->createRigidDynamic(Get_PhysXTransform(pMonster));

    // 머테리얼 환경 정의
    // 정적 마찰, 동적 마찰, 반발 계수
    PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);

    // 반지름이 0.5이고 높이가 0.3인 캡슐 헝태

    PxCapsuleGeometry MonsterCapsule = PxCapsuleGeometry(fRadius, fHalfHeight);
    PxShape* pShape = m_PhysX->createShape(MonsterCapsule, *Material);

    pRigid->attachShape(*pShape);
    m_PxScene->addActor(*pRigid);

    PxTransform             PxMonsterransform;

    m_PxMonsterTransforms.emplace_back(PxMonsterransform);
    m_MonsterCapsules.emplace_back(MonsterCapsule);
    m_MonsterRigids.emplace_back(pRigid);
    m_Monsters.emplace_back(pMonster);

    return S_OK;
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

_bool CPhysX_Manager::RayCast_PlayerDown(_vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _vector vPlayerDown = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP)) * -1.f;
    _vector vPlayerLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));
    _vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

    PxVec3 vRayDir = PxVec3(XMVectorGetX(vPlayerDown), XMVectorGetY(vPlayerDown), XMVectorGetZ(vPlayerDown));
    PxVec3 vRayPos = m_pPlayerRigid->getGlobalPose().p;// +(vRayDir * (0.86f));
    //PxVec3 vRayPos = PxVec3(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));

    return RayCast(vRayPos, vRayDir, vHitPos, vNormal, fHitDistance);
}

_bool CPhysX_Manager::RayCast_PlayerLook(_vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _vector vPlayerLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));
    PxVec3 vRayDir = PxVec3(XMVectorGetX(vPlayerLook), XMVectorGetY(vPlayerLook), XMVectorGetZ(vPlayerLook));

    PxVec3 vCenterPos = m_pPlayerRigid->getGlobalPose().p;
    PxVec3 vRayPos = m_pPlayerRigid->getGlobalPose().p + (vRayDir * (m_PlayerCapusle.radius + 0.01f));

    return RayCast(vRayPos, vRayDir, vHitPos, vNormal, fHitDistance);
}

_bool CPhysX_Manager::RayCast_PlayerHitDir(_vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _vector vPlayerDown = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP)) * -1.f;

    static _float f = 0.01f;
    if (GetKeyState(VK_UP) & 0x8000)
        f += 0.01f;

    if (GetKeyState(VK_DOWN) & 0x8000)
        f -= 0.01f;

    PxVec3 vCenterPos = m_pPlayerRigid->getGlobalPose().p;
    PxVec3 vRayDir = PxVec3(XMVectorGetX(vPlayerDown), XMVectorGetY(vPlayerDown), XMVectorGetZ(vPlayerDown));
    PxVec3 vRayPos = m_pPlayerRigid->getGlobalPose().p + (vRayDir * (0.97f));

    return RayCast(vRayPos, vRayDir, vHitPos, vNormal, fHitDistance);
}

void CPhysX_Manager::Compute_Collision(_vector* vHitDir, _float* fHitLength)
{
    if (nullptr == m_pPlayer)
        return;

    CTransform* PlayerTransformCom = m_pPlayer->Get_Transform();
    Safe_AddRef(PlayerTransformCom);

    _matrix PlayerWorldMatirx = PlayerTransformCom->Get_WorldMatrix();

    _vector vPlayerPos = PlayerWorldMatirx.r[3];
    _vector vPlayerUp = XMVector3Normalize(PlayerWorldMatirx.r[1]);

    PxVec3  vDir = { XMVectorGetX(*vHitDir), XMVectorGetY(*vHitDir), XMVectorGetZ(*vHitDir) };

    _vector vStartPos = vPlayerPos + vPlayerUp * 0.1f; // 아래
    _vector vEndPos = vPlayerPos + vPlayerUp * 1.5f;

    PxVec3 PxStartPos = { XMVectorGetX(vStartPos), XMVectorGetY(vStartPos), XMVectorGetZ(vStartPos) };
    PxVec3 PxEndPos = { XMVectorGetX(vEndPos), XMVectorGetY(vEndPos), XMVectorGetZ(vEndPos) };

    m_PxPlayerTransform = PxTransformFromSegment(PxStartPos, PxEndPos);

    _bool isCollision = false;
    _bool isGround = false;

    for (auto& Mesh : m_ColMesheGeometries)
    {
        // 충돌한 반대 방향, 충돌 깊이, 검사할 형태(EX: 캡슐), 
        if (PxComputeTriangleMeshPenetration(vDir, *fHitLength, m_PlayerCapusle, m_PxPlayerTransform, *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
        {
            // 충돌한 깊이가 0.1f 이상이면
            if (*fHitLength > 0.1f)
            {
                isCollision = true;

                // 충돌한 반대 방향
                *vHitDir = XMVector3Normalize(XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f));

                _float fRadain;
                XMStoreFloat(&fRadain, XMVector3Dot(vPlayerUp, *vHitDir));

                // 충돌 각도가 45도 아래면 땅에 붙어있는걸로 판단
                _float fAngle = XMConvertToDegrees(acosf(fRadain));
                if (fAngle <= 45.f)
                {
                    isGround = true;
                }

                // 충돌한 방향의 반대 방향으로 충돌한 깊이만큼 위치를 이동
                vPlayerPos = vPlayerPos + (*vHitDir * ((*fHitLength - 0.05f)));
                PlayerTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
            }
        }
    }

    //m_pPlayer->Set_IsCollision(isCollision);
    //m_pPlayer->Set_IsGround(isGround);

    m_pPlayerRigid->setGlobalPose(m_PxPlayerTransform);

    Safe_Release(PlayerTransformCom);
}

void CPhysX_Manager::Compute_MonsterCollision()
{
    for (_uint i = 0; i < m_Monsters.size(); ++i)
    {
        CTransform* MonsterTransformCom = m_Monsters[i]->Get_Transform();
        Safe_AddRef(MonsterTransformCom);

        _matrix MonsterWorldMatirx = MonsterTransformCom->Get_WorldMatrix();

        _vector vMonsterPos = MonsterWorldMatirx.r[3];
        _vector vMonsterUp = XMVector3Normalize(MonsterWorldMatirx.r[1]);

        PxVec3  vPxDir;
        PxReal fHitLenght;

        _vector vStartPos = vMonsterPos + vMonsterUp * 0.1f; // 아래
        _vector vEndPos = vMonsterPos + vMonsterUp * 1.5f;

        PxVec3 PxStartPos = { XMVectorGetX(vStartPos), XMVectorGetY(vStartPos), XMVectorGetZ(vStartPos) };
        PxVec3 PxEndPos = { XMVectorGetX(vEndPos), XMVectorGetY(vEndPos), XMVectorGetZ(vEndPos) };

        m_PxMonsterTransforms[i] = PxTransformFromSegment(PxStartPos, PxEndPos);

        _bool isCollision = false;
        _bool isGround = false;

        for (auto& Mesh : m_ColMesheGeometries)
        {
            
            // 충돌한 반대 방향, 충돌 깊이, 검사할 형태(EX: 캡슐), 
            if (PxComputeTriangleMeshPenetration(vPxDir, fHitLenght, m_MonsterCapsules[i], m_PxMonsterTransforms[i], *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
            {
                // 충돌한 깊이가 0.1f 이상이면
                if (fHitLenght > 0.1f)
                {
                    isCollision = true;

                    // 충돌한 반대 방향
                    _vector vDir = XMVector3Normalize(XMVectorSet(vPxDir.x, vPxDir.y, vPxDir.z, 0.f));

                    _float fRadain;
                    XMStoreFloat(&fRadain, XMVector3Dot(vMonsterUp, vDir));

                    // 충돌 각도가 45도 아래면 땅에 붙어있는걸로 판단
                    _float fAngle = XMConvertToDegrees(acosf(fRadain));
                    if (fAngle <= 45.f)
                    {
                        isGround = true;
                    }

                    // 충돌한 방향의 반대 방향으로 충돌한 깊이만큼 위치를 이동
                    vMonsterPos = vMonsterPos + (vDir * ((fHitLenght - 0.1f)));
                    MonsterTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
                }
            }
        }

        //m_Monsters[i]->Set_IsCollision(isCollision);
        //m_Monsters[i]->Set_IsGround(isGround);

        //m_MonsterRigids[i]->setGlobalPose(m_PxPlayerTransform);

        //Safe_Release(MonsterRigidbodyCom);
        Safe_Release(MonsterTransformCom);
    }

    m_MonsterCapsules.clear();
    for (auto& Rigid : m_MonsterRigids)
    {
        PhysX_RELEASE(Rigid);
    }
    m_MonsterRigids.clear();
    m_Monsters.clear();
}

void CPhysX_Manager::Reset_PhsyX()
{
    for (auto& Geometry : m_ColMesheGeometries)
    {
        PhysX_RELEASE(Geometry->triangleMesh);
        Safe_Delete(Geometry);
    }
    m_ColMesheGeometries.clear();

    for (auto& Actor : m_StaticActors)
    {
        PhysX_RELEASE(Actor);
    }
    m_StaticActors.clear();

    PhysX_RELEASE(m_pPlayerRigid);

    for (auto& Rigid : m_MonsterRigids)
    {
        PhysX_RELEASE(Rigid);
    }
    m_MonsterRigids.clear();

    Safe_Release(m_pPlayer);
    m_pPlayer = nullptr;
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

CPhysX_Manager* CPhysX_Manager::Create()
{
    CPhysX_Manager* pInstance = new CPhysX_Manager();

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

    Reset_PhsyX();

    PhysX_RELEASE(m_PxScene);

    // PhysX Extensions 닫기
    PxCloseExtensions();

    PhysX_RELEASE(m_PxDispatcher);
    PhysX_RELEASE(m_PhysX);
    PhysX_RELEASE(m_pTransport);
    PhysX_RELEASE(m_Pvd);

    // 제일 마지막에 Release
    PhysX_RELEASE(m_PxFoundation);


}