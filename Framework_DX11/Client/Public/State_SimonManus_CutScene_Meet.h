#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManus_CutScene_Meet : public CState
{
public:
    CState_SimonManus_CutScene_Meet(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManus_CutScene_Meet() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster*     m_pMonster = { nullptr };

    _uint               m_iAnimation_Turn = {};
    _uint               m_iAnimation_Talk = {};
    _uint               m_iAnimation_Hand = {};
    _uint               m_iAnimation_End = {};

public:
    static CState_SimonManus_CutScene_Meet* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END