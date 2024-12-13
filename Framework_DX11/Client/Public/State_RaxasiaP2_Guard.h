#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_Guard : public CState
{
private:
    typedef enum {
        AN_GUARD_START = 42, 
        AN_GUARD_IDLE = 12, AN_GUARD_WALK_F = 56, AN_GUARD_WALK_L = 57, AN_GUARD_WALK_R = 58
    }ANIMNUM;
public:
    CState_RaxasiaP2_Guard(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_Guard() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 2.5f };

    _uint               m_iRouteTrack = {};
    _uint               m_iMovingAnimIndex = {};
    _uint               m_iAtkCnt{};

    _uint               m_iAtkTrack{};

private:
    _bool               End_Check();

public:
    static CState_RaxasiaP2_Guard* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END