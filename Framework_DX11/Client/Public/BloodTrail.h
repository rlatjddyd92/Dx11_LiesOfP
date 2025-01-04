#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

// 공격 맞출때마다 오브젝트 매니저에 추가하는 식.
// Prototype_GameObject_Effect_BloodTrail

BEGIN(Client)
class CBloodTrail final : public CGameObject
{
public:
	enum WEAPON_TYPE { WEAPON_RAPIER, WEAPON_FLAMESWORD, WEAPON_SCISSOR_LEFT, WEAPON_SCISSOR_RIGHT, WEAPON_END };

	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		WEAPON_TYPE eStartType = { WEAPON_END };
		const _Matrix* pParentMatrix = { nullptr };	// 플레이어
		const _Matrix* pSocketMatrix = { nullptr };	// 손뼈
	} BLOODTRAIL_DESC;

private:
	CBloodTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBloodTrail(const CBloodTrail& Prototype);
	virtual ~CBloodTrail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Weapon(WEAPON_TYPE eType);
	void	Active(WEAPON_TYPE eType);
	void	DeActive();

public:
	const list<TWOPOINT>& Get_TrailPoses();
	TWOPOINT Get_PointPos(_uint iIndex);

private:
	class CTrail_Effect_TP* m_Effect[WEAPON_END] = {nullptr};
	WEAPON_TYPE m_eType = { WEAPON_END };
	_Matrix m_WorldMatrix = {};
	const _Matrix* m_pParentMatrix = { nullptr };
	const _Matrix* m_pSocketMatrix = { nullptr };;

private:
	HRESULT Ready_Effect();

public:
	static CBloodTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
