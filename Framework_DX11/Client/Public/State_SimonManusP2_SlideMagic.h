#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_SlideMagic : public CState
{
private:
    typedef enum {
        AN_SLIDE_LEFT = 43, AN_SLIDE_RIGHT = 44
    }ANIMNUM;
public:
    CState_SimonManusP2_SlideMagic(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_SlideMagic() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint           m_iCurAnim = {};
    _uint           m_iMagicCnt = {};
    
    _Vec3           m_vGoalDir = {};
    
private:
    _bool               End_Check();
    void                Effect_Check(_double CurTrackPos);

public:
    static CState_SimonManusP2_SlideMagic* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END