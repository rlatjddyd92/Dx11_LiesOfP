#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CurruptedStrongArm_HitFatal :
    public CState
{
private:
    typedef enum {
        AN_FATAL_START = 17, AN_FATAL_LOOP = 15
    }ANIMNUM;

private:
    typedef enum {
        DIR_FRONT, DIR_BEHIND
    }DIR;

public:
    CState_CurruptedStrongArm_HitFatal(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CurruptedStrongArm_HitFatal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fHitFatalTime = { 0.f };
    _float              m_fHitFatalDuration = { 3.f };

    _int                m_iAnimTrack = {};

    _bool*              m_pFatalAttacked = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_CurruptedStrongArm_HitFatal* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END