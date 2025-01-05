#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Training01_Attack : public CState
{
public:
    CState_Training01_Attack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Training01_Attack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster*     m_pMonster = { nullptr };

    _uint               m_iAnimation_Attack = {};

    _float              m_fAttackDelayTime = {};

private:
    _int                m_iColliderStartFrame = {};
    _int                m_iColliderEndFrame = {};
    void                Control_Collider();

    _bool               m_isRimLight = {};
    _bool               m_isEndRImLight = {};
    _Vec4               m_vRimLightColor = {};

public:
    static CState_Training01_Attack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END