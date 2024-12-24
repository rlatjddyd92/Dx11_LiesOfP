#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMale_RapidFire : public CState
{
private:
    typedef enum {
        AN_LINKED_START = 2, AN_LINKED_MIDDLE = 1, AN_LINKED_LAST = 0
    }ANIMNUM;
public:
    CState_RebornerMale_RapidFire(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMale_RapidFire() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iRouteTrack = {};
    _bool               m_bFire = {false};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);

public:
    static CState_RebornerMale_RapidFire* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END