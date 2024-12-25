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

    _bool               m_isStartCutScene = {};
    _bool               m_isStartPlaySound = {};

    _bool               m_bChangedToTalk = { false };

    _float              m_fDelay = {};
    _float              m_fDelay2 = {};

    _Vec3               m_vRootMoveStack{};

private:
    void                End_Check(_float fTimeDelta);

private:
    _bool               m_isShowDialog[5] = { false, };

    void                Control_Dialog(_int iFrame);

private:
    _bool               m_isSpit[2] = { false, };
    _bool               m_isBreath[3] = { false, };
    _bool               m_isKneeDown = { false };
    _bool               m_isFoot[3] = { false, };

    _int                m_iBreathFrame[3] = {};

    void                Control_Effect(_int iFrame);

public:
    static CState_SimonManus_CutScene_Meet* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END