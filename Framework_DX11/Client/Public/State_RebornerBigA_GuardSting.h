#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerBigA_GuardSting : public CState
{
private:
    typedef enum {
        AN_LINKED_START = 3, AN_LINKED_MIDDLE = 2, AN_LINKED_LAST = 5
    }ANIMNUM;
public:
    CState_RebornerBigA_GuardSting(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerBigA_GuardSting() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool               m_bSwing = { false };
    _bool               m_bSwingSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);

public:
    static CState_RebornerBigA_GuardSting* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END