#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_ThunderBall : public CState
{
private:
    typedef enum {
        AN_THUNDERBALL = 41
    }ANIMNUM;
public:
    CState_SimonManusP2_ThunderBall(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_ThunderBall() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool           m_bCreateBall = { false };

private:
    _bool               End_Check();
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_ThunderBall* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END