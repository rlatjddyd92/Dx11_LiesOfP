#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_TripleClaw_2 : public CState
{
private:
    typedef enum {
        AN_LINKED_START = 16, AN_LINKED_MIDDLE = 15, AN_LINKED_LAST = 14
    }ANIMNUM;
public:
    CState_CarcassNormal_TripleClaw_2(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_TripleClaw_2() = default;

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
    static CState_CarcassNormal_TripleClaw_2* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END