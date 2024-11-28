#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_Scratching : public CState
{
private:
    typedef enum {
        AN_SCRATCHING_L = 7, AN_SCRATCHING_R = 6
    }ANIMNUM;
public:
    CState_CarcassTail_Scratching(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_Scratching() = default;

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
    static CState_CarcassTail_Scratching* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END