#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Teleport : public CState
{
public:
    typedef struct
    {
        _bool isDie = false;
        _int iCellNum = -1;
        class CSteppingStone* pSteppingStone = { nullptr };
    }TELEPORT_DESC;

public:
    CState_Player_Teleport(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Teleport() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_TeleportStart = {};
    _uint               m_iAnimation_TeleportEnd = {};

    _int                m_iStargazerCellNum = {  };

    _double* m_pTrackPos = { nullptr };

    _float              m_fDissloveRatio = {};

    _Vec4               m_vRimLightColor = {};
    _bool               m_isEnd_Teleport = {};
    _bool               m_isAppearStartEffect = {};
    _bool               m_isFadeOut = {};
    _bool               m_isFadeIn = {};

    _bool               m_isDieTeleport = { false };

    class CSteppingStone*   m_pSteppingStone = { nullptr };
    class CStargazer*       m_pStarGazer = { nullptr };
private:
    _bool               End_Check();

    void                Update_SteppingStone(_float fTimeDelta);
    void                Update_Stargazer(_float fTimeDelta);
    void                Update_Die(_float fTimeDelta);

    void                Move_To_Stargazer();

public:
    static CState_Player_Teleport* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END