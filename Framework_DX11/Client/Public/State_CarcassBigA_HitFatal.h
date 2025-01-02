#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_HitFatal :
    public CState
{
private:
    typedef enum {
        AN_FATAL_START_F = 14, AN_FATAL_START_B = 13,
        AN_FATAL_LOOP_F = 12, AN_FATAL_LOOP_B = 11,
        AN_FATAL_END_F = 10, AN_FATAL_END_B = 9,
        AN_UP = 15
    }ANIMNUM;

private:
    typedef enum{
        DIR_BEHIND, DIR_FRONT
    }DIR;

public:
    CState_CarcassBigA_HitFatal(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_HitFatal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fHitFatalTime = { 0.f };
    _float              m_fHitFatalDuration = { 3.f };

    _uint               m_iAnimCnt = {};

    _uint               m_iDirCnt = {};

    _bool* m_pFatalAttacked = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_CarcassBigA_HitFatal* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END