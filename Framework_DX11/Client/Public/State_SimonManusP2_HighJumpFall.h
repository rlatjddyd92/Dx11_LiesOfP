#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_HighJumpFall : public CState
{
private:
    typedef enum {
        AN_HIGHJUMPFALL = 22
    }ANIMNUM;
public:
    CState_SimonManusP2_HighJumpFall(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_HighJumpFall() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _Vec4               m_vTargetDir{};
    _Vec3               m_vFlyMoveStack{};

    _Vec4               m_vRimLightColor = {};
    _bool               m_isRimLight = { false };

    _bool               m_bStampSound = { false };
    _bool               m_bStartSpot = { true };
    _bool               m_bLandSound = { false };
    _bool               m_bStampEffect = { false };
    _bool               m_bStompAttack = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Update_Rimlight(_float fTimeDelta, _double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_HighJumpFall* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END