#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManus_CutScene_Phase2 : public CState
{
public:
    CState_SimonManus_CutScene_Phase2(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManus_CutScene_Phase2() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Change = {};

    _uint               m_iAnimation_Connectgod = {};
    _uint               m_iAnimation_Begod = {};

    _bool               m_isChangePhase2 = { false };
    _bool               m_isCreatedHand= { false };
    _bool               m_isCreateDome = { false };
    _bool               m_isDistortionHand = { false };
    _bool               m_bAnimationSpeedDown = { false };
    _bool               m_bAnimationStop = { false };
    _bool               m_bAnimationRestart = { false };
    _bool               m_isFirstEmissive = {};
    _bool               m_isSecondEmissive = {};

    _float              m_fStopedTimer = { 0.f };

private:
    _bool               m_isMaxEmissive = {};
    void                Contorl_Emissive(_float fTimeDelta);

private:
    _bool               m_isShowDialog[2] = { false, };

    void                Control_Dialog(_int iFrame);

public:
    static CState_SimonManus_CutScene_Phase2* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END