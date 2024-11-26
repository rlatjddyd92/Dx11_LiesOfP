#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_ChasingSwing : public CState
{
private:
    typedef enum {
        AN_CHASINGSWING_START = 16, AN_CHASINGSWING_LOOP = 15, AN_CHASINGSWING_END = 14
    }ANIMNUM;

public:
    CState_SimonManusP2_ChasingSwing(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_ChasingSwing() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fCurrentTime = { 0.f };
    _float              m_fChaseDuration = { 2.f };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_SimonManusP2_ChasingSwing* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END