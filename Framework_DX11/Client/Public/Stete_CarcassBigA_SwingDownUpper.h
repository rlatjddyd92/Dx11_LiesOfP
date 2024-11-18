#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CStete_CarcassBigA_SwingDownUpper : public CState
{
private:
    typedef enum {
        AN_SWINGDOWNUPPER = 0
    }ANIMNUM;
public:
    CStete_CarcassBigA_SwingDownUpper(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CStete_CarcassBigA_SwingDownUpper() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _int               m_iChangeFrame = {};

    _bool*  m_pResetRootMove = { nullptr };
    _float* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

public:
    static CStete_CarcassBigA_SwingDownUpper* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END