#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_SpreadMagic : public CState
{
private:
    typedef enum {
        AN_STINGATTACK = 51
    }ANIMNUM;
public:
    CState_SimonManusP2_SpreadMagic(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_SpreadMagic() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool           m_bMagicSpread = { false };

private:
    _bool               End_Check();
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_SpreadMagic* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END