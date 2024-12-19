#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Raxasia_CutScene_Die : public CState
{
public:
    CState_Raxasia_CutScene_Die(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Raxasia_CutScene_Die() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_KneeDown = {};
    _uint               m_iAnimation_Die = {};

    _bool               m_isStartCutScene = {};
    _bool               m_isMoveDown = {};
    _bool               m_isPlayWeaponAni = {};

    _float              m_fDelay = {};

    _Vec3               m_vRootMoveStack{};

    class CRaxasia_Sword_CutScene* m_pCutSceneWeapon = { nullptr };

private:
    void                End_Check(_float fTimeDelta);

private:
    _bool               m_isShowDialog = { false };

    void                Control_Dialog(_int iFrame);

public:
    static CState_Raxasia_CutScene_Die* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END