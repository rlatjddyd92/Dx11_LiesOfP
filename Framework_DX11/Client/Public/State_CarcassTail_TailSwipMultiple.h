#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_TailSwipMultiple : public CState
{
private:
    typedef enum {
        AN_TailSwipMultiple = 2
    }ANIMNUM;
public:
    CState_CarcassTail_TailSwipMultiple(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_TailSwipMultiple() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool           m_bSwipSound = { false };
private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);

public:
    static CState_CarcassTail_TailSwipMultiple* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END