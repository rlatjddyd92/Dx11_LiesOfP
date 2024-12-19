#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_ClawRush : public CState
{
private:
    typedef enum {
        AN_CLAWRUSH = 13
    }ANIMNUM;
public:
    CState_CarcassNormal_ClawRush(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_ClawRush() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);

public:
    static CState_CarcassNormal_ClawRush* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END