#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_AttackRoute_1 :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 4,    //SwingRight_Move
        AN_ROUTE_LAST = 0      //SwingDown_Upper

    }ANIMNUM;

public:
    CState_CarcassBigA_AttackRoute_1(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_AttackRoute_1() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iRouteTrack = {};

    _bool               m_bSwingSound = { false };
    _bool               m_bStampSound = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);

public:
    static CState_CarcassBigA_AttackRoute_1* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END