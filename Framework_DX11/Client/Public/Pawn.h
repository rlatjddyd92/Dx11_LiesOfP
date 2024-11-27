#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPawn abstract : public CGameObject
{
public :
	enum PAWN_SOUND_TYPE { PAWN_SOUND_VOICE, PAWN_SOUND_EFFECT1, PAWN_SOUND_EFFECT2, PAWN_SOUND_END };

public:
    struct PAWN_STATUS
    {
        _wstring          strName{};

        _float            fHp{};
        _float            fMaxHp{};
        _float            fAtk{};
        _float            fDefence{};
        _float            fStemina{};

        _float            fGrogyPoint{};
        _float            fMaxGrogyPoint{};

        _bool             bWeakness{};

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

	class CSound*		Get_SoundCom(PAWN_SOUND_TYPE eType) { return m_pSoundCom[eType]; }

	_float				Get_MoveSpeed() { return m_fMoveSpeed; }
	void				Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg = nullptr);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT Render_Height() { return S_OK; }
    virtual HRESULT Render_LightDepth() { return S_OK; }

    virtual _bool Calc_DamageGain(_float fAtkDmg);
    virtual void Gain_Grory_Point(_float fGainGrogyPoint) { m_eStat.fGrogyPoint += fGainGrogyPoint; }

public:
    virtual void OnCollisionEnter(CGameObject* pOther) override;
    virtual void OnCollisionStay(CGameObject* pOther) override;
    virtual void OnCollisionExit(CGameObject* pOther) override;

public:
    void            Change_State(const _uint iState, void* pArg = nullptr);
    void            Set_Animation(_uint iAnimIndex, _bool IsLoop = false);
    void            Change_Animation(_uint iAnimIndex, _bool IsLoop = false, _float fDuration = 0.3f, _uint iStartFrame = 0, _bool bEitherBoundary = true);
    void            Change_Animation_Boundry(_uint iAnimIndex, _bool IsLoop = false, _float fDuration = 0.3f, _uint iStartFrame = 0);
    _uint           Get_CurrentAnimIndex();
    _int            Get_Frame();
    _double         Get_CurrentTrackPos();
    _bool           Get_EndAnim(_int iAnimIndex, _bool bIsBoundary = false);

    PAWN_STATUS*    Get_Status() { return &m_eStat; }

	void		Play_Sound(const PAWN_SOUND_TYPE eType, const TCHAR* pSoundKey);
	void		PlayRepeat_Sound(const PAWN_SOUND_TYPE eType, const TCHAR* pSoundKey);

	PAWN_STATUS* Get_Status() { return &m_eStat; }


protected:
	class CShader*		m_pShaderCom = { nullptr };
	class CModel*		m_pModelCom = { nullptr };
	class CNavigation*	m_pNavigationCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };
	class CFsm*			m_pFsmCom = { nullptr };
	class CRigidBody*	m_pRigidBodyCom = { nullptr };
	class CSound*		m_pSoundCom[PAWN_SOUND_END] = { nullptr, };

    _bool            m_isGravity = { false };

    _float            m_fMoveSpeed = {};

    _float            m_fEmissiveMask = {};

    _Vec4            m_vRimLightColor = {};

    _Vec3            m_vVelocity = {};

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