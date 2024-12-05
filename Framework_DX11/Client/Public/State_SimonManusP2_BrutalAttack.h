#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_BrutalAttack : public CState
{
private:
    typedef enum {
        AN_BRUTALATTACK = 24
    }ANIMNUM;
public:
    CState_SimonManusP2_BrutalAttack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_BrutalAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool               m_bSwing = { false };

    _bool               m_bSwingSound = { false };
    _bool               m_bStampSound = { false };
     
private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_BrutalAttack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END