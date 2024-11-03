#pragma once
#include "Base.h"

// 디버그 모드에서 릭을 잡기위한 crtdebug 때문에 PhysX의 new 재정의에서 오류가 남
// 피직스  헤더파일을 포함하기 전에 undef로 new 재정의를 잠시 해제함
#ifdef _DEBUG
#undef new
#endif

#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

// PhysX 헤더파일을 포함한 후 매크로 재정의를 다시 활성화
#ifdef _DEBUG
#define new DBG_NEW
#endif

using namespace physx;

BEGIN(Engine)

class CTransform;
class CGameObject;

class ENGINE_DLL CPhysX_Manager :
    public CBase
{
    CPhysX_Manager();
    virtual ~CPhysX_Manager() = default;

public:
    HRESULT Initialize();
    void    PhysX_Update(_float fTimeDelta);

public:
    HRESULT AddPhysX_StaticMesh(class CGameObject* pObject, _wstring strModelName);
    HRESULT SetUp_Player(class CGameObject* pPlayer);

    HRESULT Add_Monster(class CGameObject* pMonster);

    _bool   RayCast(PxVec3 vRayPos, PxVec3 vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance);
    _bool   RayCast(_vector vRayPos, _vector vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance);

    _bool   RayCast_PlayerDown(_vector* vHitPos, _vector* vNormal, _float* fHitDistance);
    _bool   RayCast_PlayerLook(_vector* vHitPos, _vector* vNormal, _float* fHitDistance);
    _bool   RayCast_PlayerHitDir(_vector* vHitPos, _vector* vNormal, _float* fHitDistance);

    void    Compute_Collision(_vector* vHitDir, _float* fHitLength);
    void    Compute_MonsterCollision();

    void    Reset_PhsyX();

private:
    // PhysX Foundation 객체 - PhysX 시스템의 기본 인스턴스. 메모리 할당자와 에러 콜백을 초기화하는데 사용
    PxFoundation* m_PxFoundation = { nullptr };

    // PVD(PhysX Visual Debugger) 객체 - PhysX의 디버깅 정보를 시각적으로 볼 수 있게 해주는 도구
    PxPvd* m_Pvd = { nullptr };
    PxPvdTransport* m_pTransport = { nullptr };
    PxPvdSceneClient* m_PvdClient = { nullptr };

    // PhysX SDK 객체 - 물리 엔진의 핵심. 물리 시뮬레이션을 위한 모든 설정 및 관리 담당
    PxPhysics* m_PhysX = { nullptr };

    // 물리 씬 객체 - 물리 시뮬레이션이 이루어지는 공간. 충돌, 중력, 힘 등의 상호작용을 처리
    PxScene* m_PxScene = { nullptr };

    // PhysX 할당자 및 에러 콜백 - 메모리 할당과 에러 처리를 위한 콜백 클래스
    PxDefaultAllocator m_PxAllocator;
    PxDefaultErrorCallback m_PXErrorCallback;

    // PVD 연결 설정 - 디버깅 정보를 전송할 IP 주소
    std::string m_pvdIPAddress = "127.0.0.1";

    // 통신하기 위한 포트번호 - PVD와 통신하는 포트 번호 - 기본 5425
    _int m_iPvdPortNumber = 5425;

    // 10초 안에 연결 안 되면 포기함 - PVD 연결 타임아웃 설정
    _uint m_iPvdTimeOutSeconds = 10;

    // CPU 디스패처(워커 스레드) 객체 - 물리 시뮬레이션을 병렬로 처리하기 위한 CPU 디스패처
    PxDefaultCpuDispatcher* m_PxDispatcher = { nullptr };

    vector<PxTriangleMeshGeometry*> m_ColMesheGeometries;

    // 임시로 사용되는 정적 Actor 목록
    vector<PxRigidStatic*> m_StaticActors;

    class CGameObject* m_pPlayer = { nullptr };
    PxRigidDynamic* m_pPlayerRigid = { nullptr };
    PxCapsuleGeometry       m_PlayerCapusle;

    PxTransform             m_PxPlayerTransform;

    vector<class CGameObject*>  m_Monsters;
    vector<PxRigidDynamic*>     m_MonsterRigids;
    vector<PxCapsuleGeometry>   m_MonsterCapsules;
    vector<PxTransform>         m_PxMonsterTransforms;

private:
    // 주어진 게임 오브젝트의 PhysX 변환 정보를 얻기 위한 함수
    PxTransform Get_PhysXTransform(CGameObject* pObject);


public:
    static CPhysX_Manager* Create();
    virtual void Free() override;
};

END

/* PhysX 라이브러리 파일들
PhysX_64.lib
PhysXCommon_64.lib
PhysXCooking_64.lib
PhysXExtensions_static_64.lib
PhysXFoundation_64.lib
PhysXPvdSDK_static_64.lib */