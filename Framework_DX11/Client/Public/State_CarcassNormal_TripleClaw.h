#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_TripleClaw : public CState
{
private:
    typedef enum {
        AN_LINKED_START = 9, AN_LINKED_MIDDLE = 8, AN_LINKED_LAST = 7
    }ANIMNUM;
public:
    CState_CarcassNormal_TripleClaw(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_TripleClaw() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iRouteTrack = {};

    _bool           m_bSwingSound = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);

public:
    static CState_CarcassNormal_TripleClaw* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END