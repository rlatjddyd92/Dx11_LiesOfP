#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_ThrowItem : public CState
{
    enum WALK
    {
        WALK_B, WALK_F, WALK_L, WALK_R, WALK_END
    };

public:
    CState_Player_ThrowItem(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_ThrowItem() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Walk[WALK_END] = {};
    _uint               m_iAnimation_Throw = {};

    _Vec4               m_vMoveDir = {};

    _float              m_fMoveSpeed = {};

    _double*            m_pTrackPos = { nullptr };

    _bool               m_isCreateItem = { false };
    _bool               m_isThrowItem = { false };

private:
    _bool               Move(_float fTimeDelta);
    _bool               End_Check();

private:
    _bool               m_isPlaySound = {};

public:
    static CState_Player_ThrowItem* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END