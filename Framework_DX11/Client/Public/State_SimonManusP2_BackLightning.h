#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_BackLightning : public CState
{
private:
    typedef enum {
        AN_BACKLIGHTNING = 47
    }ANIMNUM;
public:
    CState_SimonManusP2_BackLightning(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_BackLightning() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool           m_bStingSound = { false };

    _bool           m_bLightning = { false };
    
private:
    _bool               End_Check();
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_BackLightning* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END