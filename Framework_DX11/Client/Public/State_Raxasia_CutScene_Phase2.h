#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Raxasia_CutScene_Phase2 : public CState
{
public:
    CState_Raxasia_CutScene_Phase2(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Raxasia_CutScene_Phase2() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Phase2 = {};

    _bool               m_isStartCutScene = {};
    _bool               m_isChangePhase2Model = {};
    _bool               m_isOnGroundWeapon = {};
    _bool               m_isChangeWeaponPos = {};
    _bool               m_isReConnetWeaponBone = {};
    _bool               m_isPlayWeaponChangeAnim = {};
    _bool               m_isConneectLeftHandShield = {};

    _float              m_fDelay = {};

    _Vec3               m_vRootMoveStack{};

    class CRaxasia_Helmet_CutScene* m_pCutSceneHelmet = { nullptr };
    class CRaxasia_Sword_CutScene*  m_pCutSceneWeapon = { nullptr };
    class CWeapon*                  m_pShieldWeapon = { nullptr };
private:
    void                End_Check();

public:
    static CState_Raxasia_CutScene_Phase2* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END