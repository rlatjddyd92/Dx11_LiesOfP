#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CurruptedStrongArm_SwipAttack : public CState
{
private:
    typedef enum {
        AN_SWIPATTACK = 2
    }ANIMNUM;
public:
    CState_CurruptedStrongArm_SwipAttack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CurruptedStrongArm_SwipAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    _bool               m_bSwingSound = { false };
    _bool               m_bTentaSound = { false };
    _bool               m_bTentaSound_2 = { false };

    class CMonster*     m_pMonster = { nullptr };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);

public:
    static CState_CurruptedStrongArm_SwipAttack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END