#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Die : public CState
{
public:
    enum DIE_TYPE
    {
        HIT_B, HIT_F, DOWN_B, DOWN_F, CURSE, DIE_END
    };

    typedef struct
    {
        _bool isFront = false;
        _Vec3 vHitPos = {};
    }DIE_DESC;

public:
    CState_Player_Die(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Die() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Die[DIE_END] = {};

    _double*            m_pTrackPos = { nullptr };

    _bool               m_isDeadEnd = {};
    _float              m_fDieTime = {};


    _bool               m_isAppearStartEffect = {};
    _bool               m_isFadeOut = {};

    _float              m_fDissloveRatio = {};

    _Vec4               m_vRimLightColor = {};

    _bool               m_bSoundPlay = { false };
    _bool               m_bSoundPlay2 = { false };

    _int                m_iAnimIndex = { 0 };
    _int                m_iSoundPlayFrame = { 0 };
private:
    _uint               Choice_DieAnim(DIE_DESC* pDesc);
    _bool               End_Check();

public:
    static CState_Player_Die* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END