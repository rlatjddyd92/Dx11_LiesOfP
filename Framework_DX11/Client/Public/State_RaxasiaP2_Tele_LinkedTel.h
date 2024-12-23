#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_Tele_LinkedTel : public CState
{
private:
    typedef enum {
        AN_LINKED_FIRST = 18, AN_LINKED_SECOND = 19, AN_UPPERSLASH = 38
    }ANIMNUM;

public:
    CState_RaxasiaP2_Tele_LinkedTel(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_Tele_LinkedTel() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fDistance = {};
    _float              m_fAccelSpeed = { 10.f };
    _float              m_fCurrentTime = { 0.f };
    _float              m_fChaseDuration = { 1.5f };

    _bool               m_bSwing = { false };
    _bool               m_bAccel = { false };

    _bool               m_bSwingSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_Tele_LinkedTel* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END