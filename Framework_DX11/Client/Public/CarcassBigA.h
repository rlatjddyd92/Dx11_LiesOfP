#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CFSM;
END

BEGIN(Client)

class CCarcassBigA final : public CMonster
{
public:
	typedef struct
	{
		_bool* pIsEndAnim { nullptr };
		_bool* pIsResetRootMove { nullptr };
		_double* pGrogyTrackPos { nullptr };
	}FSMSTATE_DESC;

public:
	enum COLLIDERTYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

public:
	enum CARCASS_BIGA_STATE {
		SWING = MONSTER_STATE_END,
		IMPACT,

	};

private:
	CCarcassBigA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCarcassBigA(const CCarcassBigA& Prototype);
	virtual ~CCarcassBigA() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();


public:
	static CCarcassBigA* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END