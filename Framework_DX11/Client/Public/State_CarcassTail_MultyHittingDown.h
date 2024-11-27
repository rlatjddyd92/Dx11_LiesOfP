#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_MultyHittingDown : public CState
{
private:
    typedef enum {
        AN_MULTYHITTINGDOWN = 4
    }ANIMNUM;
public:
    CState_CarcassTail_MultyHittingDown(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_MultyHittingDown() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_CarcassTail_MultyHittingDown* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END