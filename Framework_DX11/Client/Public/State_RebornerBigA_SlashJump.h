#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerBigA_SlashJump : public CState
{
private:
    typedef enum {
        AN_SWIPATTACK = 10
    }ANIMNUM;
public:
    CState_RebornerBigA_SlashJump(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerBigA_SlashJump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool           m_bSwingSound = { false };

    _bool           m_bFrameDelay = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);

public:
    static CState_RebornerBigA_SlashJump* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END