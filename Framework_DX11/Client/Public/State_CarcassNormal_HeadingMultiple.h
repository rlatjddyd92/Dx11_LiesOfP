#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_HeadingMultiple : public CState
{
private:
    typedef enum {
        AN_ROTATE_HEADING = 20, AN_HEADING = 21
    }ANIMNUM;
public:
    CState_CarcassNormal_HeadingMultiple(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_HeadingMultiple() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iRouteTrack = {};

    _bool               m_bSwingSound = { false };
    _bool               m_bHeadingSound = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);

public:
    static CState_CarcassNormal_HeadingMultiple* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END