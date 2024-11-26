#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_SwingMultiple : public CState
{
private:
    typedef enum {
        AN_SWINGMULTIPLE = 22
    }ANIMNUM;
public:
    CState_SimonManusP1_SwingMultiple(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_SwingMultiple() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimCnt = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_SimonManusP1_SwingMultiple* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END