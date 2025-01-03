#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMaleFire_Breath : public CState
{
private:
    typedef enum {
        AN_LINKED_START = 18, AN_LINKED_LAST = 19
    }ANIMNUM;
public:
    CState_RebornerMaleFire_Breath(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMaleFire_Breath() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool               m_bBreath = { false };
    _bool               m_bSwingSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);

public:
    static CState_RebornerMaleFire_Breath* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END