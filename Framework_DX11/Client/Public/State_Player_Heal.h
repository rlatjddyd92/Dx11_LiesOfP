#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Heal : public CState
{
    enum WEAPONTYPE { WEP_OH, WEP_TH, WEP_END };
    enum WALK
    {
        WALK_B, WALK_BL, WALK_BR, WALK_F, WALK_FL, WALK_FR, WALK_L, WALK_R, WALK_END
    };

public:
    CState_Player_Heal(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Heal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Walk[2][WALK_END] = {};
    _uint               m_iAnimation_Heal = {};

    _Vec4               m_vMoveDir = {};

    _float              m_fMoveSpeed = {};
    _float              m_fRecoveryAmount = {};

    _double*            m_pTrackPos = { nullptr };

    _bool               m_isRecoveryHP = { false };

private:
    _bool               Move(_float fTimeDelta);
    _bool               End_Check();

private:
    _bool               m_isPlaySound = {};

public:
    static CState_Player_Heal* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END