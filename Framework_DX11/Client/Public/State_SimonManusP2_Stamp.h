#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

//���� �ʿ�
class CState_SimonManusP2_Stamp : public CState
{
private:
    typedef enum {
        AN_STAMP = 25, AN_STAMP_MOVE = 26, AN_SPINBLAST = 27
    }ANIMNUM;

public:
    CState_SimonManusP2_Stamp(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_Stamp() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iCurStartAnim = {};

    _bool               m_bStamp = { false };
    _bool               m_bStampEffect = { false };
    _bool               m_bWeaponSpin = { false };
    _bool               m_bBlast = { false };
    _bool               m_bAbsorb = { false };
    _bool               m_bStampSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_Stamp* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END