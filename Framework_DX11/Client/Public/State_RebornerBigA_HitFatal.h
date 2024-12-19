#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerBigA_HitFatal :
    public CState
{
private:
    typedef enum {
        AN_FATAL_START = 42, AN_FATAL_LOOP = 40
        , AN_DOWN_B = 25, AN_DOWN_F = 26, AN_UP_B = 27, AN_UP_F = 28
    }ANIMNUM;

private:
    typedef enum {
        DIR_BEHIND, DIR_FRONT
    }DIR;

public:
    CState_RebornerBigA_HitFatal(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerBigA_HitFatal() = default;

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

private:
    _bool               End_Check();

public:
    static CState_RebornerBigA_HitFatal* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END