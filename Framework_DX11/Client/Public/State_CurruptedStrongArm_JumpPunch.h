#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CurruptedStrongArm_JumpPunch : public CState
{
private:
    typedef enum {
        AN_PUNCHTWICE = 0, AN_JUMP = 3
    }ANIMNUM;
public:
    CState_CurruptedStrongArm_JumpPunch(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CurruptedStrongArm_JumpPunch() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);

public:
    static CState_CurruptedStrongArm_JumpPunch* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END