#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "ColliderObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CFSM;
END

BEGIN(Client)

class CMonster_Training01 final : public CMonster
{
public:
	enum TRAINING_STATE
	{
		ATTACK = CMonster::MONSTER_STATE_END

		, TRAINING_STATE_END
	};

private:
	CMonster_Training01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Training01(const CMonster_Training01& Prototype);
	virtual ~CMonster_Training01() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CColliderObject* m_pColliderObject = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

public:
	static CMonster_Training01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END