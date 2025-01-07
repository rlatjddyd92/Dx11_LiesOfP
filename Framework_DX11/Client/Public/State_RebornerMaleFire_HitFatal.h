#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMaleFire_HitFatal :
    public CState
{
private:
    typedef enum {
        AN_FATAL_START = 17, AN_FATAL_LOOP = 16
        , AN_DOWN_B = 2, AN_DOWN_F = 3, AN_UP_B = 4, AN_UP_F = 5
    }ANIMNUM;

private:
    typedef enum {
        DIR_BEHIND, DIR_FRONT
    }DIR;

public:
    CState_RebornerMaleFire_HitFatal(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMaleFire_HitFatal() = default;

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

    _int                m_iDirCnt = {};

    _bool*              m_pFatalAttacked = { nullptr };
    _bool*              m_pBlockDead = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_RebornerMaleFire_HitFatal* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END