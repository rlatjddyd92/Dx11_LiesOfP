#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_Leap : public CState
{
private:
    typedef enum {
        AN_LEAP = 11
    }ANIMNUM;
public:
    CState_CarcassTail_Leap(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_Leap() = default;

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
    static CState_CarcassTail_Leap* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END