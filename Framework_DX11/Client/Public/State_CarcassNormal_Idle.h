#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 95, AN_TURN_LEFT = 123, AN_TURN_RIGHT = 124
        , AN_RUN = 121, AN_WALK = 126
    }ANIMNUM;

public:
    CState_CarcassNormal_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool*              m_pFirstMeetCheck = { nullptr };
    _bool*              m_pDiscover = { nullptr };

    _float              m_fRecognitionDist = { 15.f };
    _float              m_fDiscoverDist = { 4.f };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 0.4f };

    _bool               m_bRunning = { false };
    _bool               m_bWalk = { false };

    _float              m_fRunSpeed = { 2.5f };
    _float              m_fWalkSpeed = { 1.f };

    _float              m_fRunningWeights = { 4.f };

    _float              m_fNeedDist_ForAttack = { 3.5f };

    _uint               m_iAtkCnt{};

private:
    void                Calc_Act_Attack(_float fTimeDelta, _float fDist);

public:
    static CState_CarcassNormal_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END