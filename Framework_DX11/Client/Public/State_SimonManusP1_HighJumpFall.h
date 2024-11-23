#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_HighJumpFall : public CState
{
private:
    typedef enum {
        AN_HIGHJUMPFALL = 21
    }ANIMNUM;
public:
    CState_SimonManusP1_HighJumpFall(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_HighJumpFall() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Idle = {};
    _uint               m_iIdleCount = {};

    _float              m_fCurrentTime = { 0.f };

    _Vec4               m_vTargetDir{};
    _Vec3               m_vFlyMoveStack{};

    _bool               m_isPrevChance = { false };
    _bool               m_isActivatedSpecial = { false };

    _float              m_fParalizeTime = { 0.f };
    _float              m_fParalizeDuration = { 3.f };

    _bool* m_pResetRootMove = { nullptr };

    _uint               m_iAnimCnt = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_SimonManusP1_HighJumpFall* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END