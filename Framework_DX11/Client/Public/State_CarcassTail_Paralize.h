#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_Paralize : public CState
{
private:
    typedef enum {
        AN_PARALYZE_START = 28, AN_PARALYZE_LOOP = 27, AN_PARALYZE_END = 26
    }ANIMNUM;
public:
    CState_CarcassTail_Paralize(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_Paralize() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fParalizeTime = { 0.f };
    _float              m_fParalizeDuration = { 3.f };

    _uint               m_iAnimCnt = {};

private:
    _bool               End_Check();

public:
    static CState_CarcassTail_Paralize* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END