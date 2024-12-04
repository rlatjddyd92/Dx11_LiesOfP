#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_SummonHand : public CState
{
private:
    typedef enum {
        AN_SUMMONHAND = 49
    }ANIMNUM;
public:
    CState_SimonManusP2_SummonHand(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_SummonHand() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

private:
    _bool               End_Check();
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_SummonHand* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END