#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_LOSwingRight : public CState
{
private:
    typedef enum {
        AN_LOSWINGRIGHT = 7
    }ANIMNUM;
public:
    CState_CarcassBigA_LOSwingRight(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_LOSwingRight() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _Vec4               m_vRimLightColor = {};

    _bool               m_bSwingSound = { false };

    _bool               m_isRimLight = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);
    void                Update_Rimlight(_float fTimeDelta, _double CurTrackPos);

public:
    static CState_CarcassBigA_LOSwingRight* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END