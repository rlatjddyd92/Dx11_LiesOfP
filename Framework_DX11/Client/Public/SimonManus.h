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

class CSimonManus final : public CMonster
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
		ATK_AVOIDSWING = MONSTER_STATE_END, ATK_CHASINGSWING, ATK_JUMPTOSWING, ATK_STAMP
		, ATK_STING, ATK_HIGHJUMPFALL, ATK_SWINGDOWN_L, ATK_SWINGDOWN_R
		, ATK_SWIPMULT_L, ATK_SWIPMULT_R, ATK_SWINGMULTIPLE
	};

private:
	CSimonManus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSimonManus(const CSimonManus& Prototype);
	virtual ~CSimonManus() = default;

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

	class CWeapon*	m_pWeapon = { nullptr };
	class CFsm*		m_pExtraFsmCom = { nullptr };	//2페이즈 fsm
	class CModel*	m_pExtraModelCom = { nullptr };	//2페이즈 model

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Weapon();


public:
	static CSimonManus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END