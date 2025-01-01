#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_WheelWind : public CState
{
private:
    typedef enum {
        AN_SWINGRIGHT = 2
    }ANIMNUM;
public:
    CState_CarcassBigA_WheelWind(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_WheelWind() = default;

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
    void                Sound_Check(_double CurTrackPos);
public:
    static CState_CarcassBigA_WheelWind* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END