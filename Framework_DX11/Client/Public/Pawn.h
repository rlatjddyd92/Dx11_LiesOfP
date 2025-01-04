#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPawn abstract : public CGameObject
{
public:
    enum PAWN_SOUND_TYPE { PAWN_SOUND_VOICE, PAWN_SOUND_EFFECT1, PAWN_SOUND_EFFECT2, PAWN_SOUND_END };

public:
    struct PAWN_STATUS
    {
        _wstring          strName{};

        _float            fHp{};
        _float            fMaxHp{};
        _float            fAtk{};

        _float            fGrogyPoint{};
        _float            fMaxGrogyPoint{};

        _bool             bWeakness{}; // 체력바 하얀 표시
        _bool             bFatalAttack{}; // 발톱 표시

        _float            fAtkDmg{};
    };

protected:
    CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPawn(const CPawn& Prototype);
    virtual ~CPawn() = default;

public:
    class CModel* Get_Model() { return m_pModelCom; }
    class CRigidBody* Get_RigidBody() { return m_pRigidBodyCom; }
    class CCollider* Get_Collider() { return m_pColliderCom; }
    class CFsm* Get_Fsm() { return m_pFsmCom; }
    class CNavigation* Get_Navigation() { return m_pNavigationCom; }

    class CSound* Get_SoundCom(PAWN_SOUND_TYPE eType) { return m_pSoundCom[eType]; }

public:
    _float				Get_MoveSpeed() { return m_fMoveSpeed; }
    void				Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }

    void                Set_RimLightColor(_Vec4 vRimLight) { m_vRimLightColor = vRimLight; }

    HIT_TYPE            Get_HitType() { return m_eHitType; }

    void                Play_Animation() { m_isPlayAnimation = true; }
    void                Stop_Animation() { m_isPlayAnimation = false; }

    void                SetUp_Die() { m_bDieState = true; }
    void			    Reset_Die() { m_bDieState = false; }
    _bool			    Get_IsDieState() { return m_bDieState; }

    _float              Get_EmissiveMask() { return m_fEmissiveMask; }
    void                Set_EmissiveMask(_float fEmissive) { m_fEmissiveMask = fEmissive; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg = nullptr);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT Render_Height() { return S_OK; }
    virtual HRESULT Render_LightDepth() { return S_OK; }

public:
    virtual _bool   Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos = { 0.f,0.f,0.f }, _uint iHitType = HIT_END, _uint iAttackStrength = ATK_END, CGameObject* pAttacker = nullptr) override;

    virtual void    Start_CutScene(_uint iCutSceneNum) {};
    virtual void    End_CutScene(_uint iCutSceneNum) {};
    virtual void    Change_Model(_uint iModelNum) {};

public:
    virtual void OnCollisionEnter(CGameObject* pOther) override;
    virtual void OnCollisionStay(CGameObject* pOther) override;
    virtual void OnCollisionExit(CGameObject* pOther) override;

public:
    virtual void    Change_State(const _uint iState, void* pArg = nullptr);
    void            Set_Animation(_uint iAnimIndex, _bool IsLoop = false);
    void            SetUp_Animation(_uint iAnimIndex, _bool IsLoop = false, _uint iStartFrame = 0, _bool bEitherBoundary = true);
    void            SetUp_Animation_Boundry(_uint iAnimIndex, _bool IsLoop = false, _uint iStartFrame = 0);
    void            Change_Animation(_uint iAnimIndex, _bool IsLoop = false, _float fDuration = 0.3f, _uint iStartFrame = 0, _bool bEitherBoundary = true, _bool bSameChange = false);
    void            Change_Animation_Boundry(_uint iAnimIndex, _bool IsLoop = false, _float fDuration = 0.3f, _uint iStartFrame = 0);
    _uint           Get_CurrentAnimIndex();
    _uint           Get_CurrentAnimIndex_Boundry();
    _int            Get_Frame(_bool isBoundry = false);
    _double         Get_CurrentTrackPos();
    _float          Get_CurrentTrackEnd();
    _bool           Get_EndAnim(_int iAnimIndex, _bool bIsBoundary = false);

    void		    Play_Sound(const PAWN_SOUND_TYPE eType, const TCHAR* pSoundKey, _bool isLoop = false);
    void		    PlayRepeat_Sound(const PAWN_SOUND_TYPE eType, const TCHAR* pSoundKey);
    void            Stop_Sound(const PAWN_SOUND_TYPE eType);

    void            Set_Respawn_Cell_Num(_int iNum) { m_iRespawn_Cell_Num = iNum; }

    PAWN_STATUS* Get_Status() { return &m_eStat; }

    _int            Get_UFBIndex(_uint UFB_Type);

    _Vec4            Calc_CenterPos(_bool isUsingBone = false);
    _Vec3            Calc_BoneWorldPos(const _char* szBoneName);

protected:
    class CShader* m_pShaderCom = { nullptr };
    class CModel* m_pModelCom = { nullptr };
    class CNavigation* m_pNavigationCom = { nullptr };
    class CCollider* m_pColliderCom = { nullptr };
    class CFsm* m_pFsmCom = { nullptr };
    class CRigidBody* m_pRigidBodyCom = { nullptr };
    class CSound* m_pSoundCom[PAWN_SOUND_END] = { nullptr, };

    _bool            m_isGravity = { false };
    _bool            m_isPlayAnimation = { true };
    _bool            m_isCutScene = { false };
    _bool            m_isStartDisslove = { false };

    _bool		     m_bDieState = { false };

    _int             m_iRespawn_Cell_Num = { -1 };
    _int             m_iInitRoomNum = { -1 };

    _float           m_fMoveSpeed = {};
    _float           m_fEmissiveMask = {};
    _float			 m_fDissloveRatio = {};


    _Vec4            m_vRimLightColor = {};

    _Vec3            m_vVelocity = {};
    _Vec3            m_vCenterOffset = {};

    HIT_TYPE         m_eHitType = {};

    //스테이터스 부분
    PAWN_STATUS         m_eStat{};

protected:
	HRESULT Bind_WorldViewProj();
	HRESULT Ready_Components();

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END