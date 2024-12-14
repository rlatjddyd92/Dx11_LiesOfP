#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Raxasia_CutScene_Meet : public CState
{
public:
    CState_Raxasia_CutScene_Meet(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Raxasia_CutScene_Meet() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Meet = {};

    _bool               m_isStartCutScene = {};
    _float              m_fDelay = {};

    _Vec3               m_vRootMoveStack{};

private:
    void                End_Check();

public:
    static CState_Raxasia_CutScene_Meet* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END