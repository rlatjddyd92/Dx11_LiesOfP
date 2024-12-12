#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_Running : public CState
{
private:
    typedef enum {
        AN_RUNNING = 39, AN_DASH_L = 16, AN_DASH_R = 17
    }ANIMNUM;

public:
    CState_RaxasiaP2_Running(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_Running() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fDistance = {};

    _int                m_iDashAnimIndex = {};

    _bool               m_bRunningWise = { false }; //방향 구분용으로 사용. false면 왼, true면 오른
    _bool               m_bSwing = { false };
    _bool               m_bSwingSound = { false };

    _uint               m_iRouteTrack = {};

    _uint               m_iNextStateNum = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_Running* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END