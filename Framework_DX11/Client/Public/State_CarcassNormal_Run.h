#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_Run : public CState
{
private:
    typedef enum {
        AN_RUN = 61
    }ANIMNUM;

public:
    CState_CarcassNormal_Run(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_Run() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fRunSpeed = { 3.5f };

public:
    static CState_CarcassNormal_Run* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END