#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include"ColliderObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CFSM;
END

BEGIN(Client)

class CSimonManusP1 final : public CMonster
{
public:
	typedef struct
	{
		_bool* pIsEndAnim { nullptr };
		_bool* pIsResetRootMove { nullptr };
		_bool* pColliderCtrs{ nullptr };
		
		_double* pGrogyTrackPos { nullptr };
	}FSMSTATE_DESC;

public:
	enum COLLIDERTYPE { TYPE_LEFTHAND, TYPE_RIGHTHAND, TYPE_END };

public:
	enum SIMONMANUS_STATE {
		ATK_SWING = MONSTER_STATE_END, ATK_MOVEVASH, ATK_AVOIDSWING, ATK_CHASINGSWING
		, ATK_STAMP, ATK_JUMPTOSWING, ATK_STING, ATK_UPPER
		, ATK_STOP,
	};

private:
	CSimonManusP1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSimonManusP1(const CSimonManusP1& Prototype);
	virtual ~CSimonManusP1() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CGameObject*			m_pColliderObject[TYPE_END] = { nullptr, nullptr };
	_bool					m_bColliderCtrs[TYPE_END] = {true,  true};
	vector<CGameObject*>	CollObjRenderP{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();


public:
	static CSimonManusP1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END