#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_TailSwip : public CState
{
private:
    typedef enum {
        AN_TAILSWIP_L = 0, AN_TAILSWIP_R = 1
    }ANIMNUM;
public:
    CState_CarcassTail_TailSwip(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_TailSwip() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint           m_iCurAnim = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_CarcassTail_TailSwip* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END