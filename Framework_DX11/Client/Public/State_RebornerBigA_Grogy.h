#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerBigA_Grogy : public CState
{
private:
    typedef enum {
        AN_GROGY_START = 45, AN_GROGY_LOOP = 44, AN_GROGY_END = 43
    }ANIMNUM;
public:
    CState_RebornerBigA_Grogy(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerBigA_Grogy() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fGrogyTime = { 0.f };
    _float              m_fGrogyDuration = { 3.f };

    _double* m_pTrackPos = { nullptr };

    _uint               m_iAnimCnt = {};

private:
    _bool               End_Check();

public:
    static CState_RebornerBigA_Grogy* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END