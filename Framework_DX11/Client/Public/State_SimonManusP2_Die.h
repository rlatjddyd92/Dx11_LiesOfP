#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_Die : public CState
{
private:
    typedef enum {
        AN_DIE = 7
    }ANIMNUM;

public:
    CState_SimonManusP2_Die(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_Die() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

private:
    _bool               End_Check();

private:
    _bool               m_isEnd_DownEmissive = { false };
    void                Contorl_Emissive(_float fTimeDelta);

private:
    _float              m_fDisapperDomeTime = {};
    _bool               m_isDisappearDome = { false };

public:
    static CState_SimonManusP2_Die* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END